#include "stdafx.h"
#include "DirectXDevice.h"
#include "Debug.h"

using namespace DirectXHelper;

void DirectXDevice::Initalize(HWND hwnd) {
	assert(hwnd);
	assert(!device_);
	assert(!commandQueue_);
	hwnd_ = hwnd;
	// ウィンドウ情報を取得
	WINDOWINFO windowInfo{};
	windowInfo.cbSize = sizeof(WINDOWINFO);
	if (!GetWindowInfo(hwnd, &windowInfo)) { assert(false); }
	swapChainWidth_ = windowInfo.rcClient.right - windowInfo.rcClient.left;
	swapChainHeight_ = windowInfo.rcClient.bottom - windowInfo.rcClient.top;
	CreateDevice();
	CreateCommands();
	CreateDescriptorHeap();
	CreateSwapChain();
	CreateDepthStencilBuffer();
	CreateImGui();
}

void DirectXDevice::Finalize() {
	WaitForGPU();
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CloseHandle(fenceEvent_);
}

void DirectXDevice::BeginFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void DirectXDevice::StertScreenRendering() {
	auto backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList_->ResourceBarrier(1, &barrier);

	commandList_->OMSetRenderTargets(
		1,
		&rtvHandle_[backBufferIndex].cpu,
		false,
		&dsvHandle_.cpu);

	float clearColor[4] = { 0.1f,0.3f,0.6f,1.0f };
	commandList_->ClearRenderTargetView(rtvHandle_[backBufferIndex].cpu, clearColor, 0, nullptr);
	commandList_->ClearDepthStencilView(dsvHandle_.cpu, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	D3D12_VIEWPORT viewport{
					.TopLeftX = 0.0f,
					.TopLeftY = 0.0f,
					.Width = static_cast<float>(swapChainWidth_),
					.Height = static_cast<float>(swapChainHeight_),
					.MinDepth = 0.0f,
					.MaxDepth = 1.0f
	};
	commandList_->RSSetViewports(1, &viewport);

	D3D12_RECT scissorRect{
		.left = 0,
		.top = 0,
		.right = static_cast<LONG>(swapChainWidth_),
		.bottom = static_cast<LONG>(swapChainHeight_)
	};
	commandList_->RSSetScissorRects(1, &scissorRect);

	ID3D12DescriptorHeap* ppHeaps[] = { commonHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, ppHeaps);
}

void DirectXDevice::FinishScreenRendering() {
	auto backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	ImGui::Render();
	ID3D12DescriptorHeap* ppHeaps[] = { commonHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, ppHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		swapChainResource_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	commandList_->ResourceBarrier(1, &barrier);

	WaitForGPU();
	SubmitCommandList();
	GetSwapChain()->Present(1, 0);

	auto nextBackBufferIndex = GetSwapChain()->GetCurrentBackBufferIndex();
	ResetCommandList(nextBackBufferIndex);
}

void DirectXDevice::SubmitCommandList() {
	CHECK_HRESULT(commandList_->Close());
	ID3D12CommandList* cmdList[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, cmdList);
}

void DirectXDevice::WaitForGPU() {
	++fenceValue_;
	CHECK_HRESULT(commandQueue_->Signal(fence_.Get(), fenceValue_));
	if (fence_->GetCompletedValue() < fenceValue_) {
		CHECK_HRESULT(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

void DirectXDevice::ResetCommandList(uint32_t allocatorIndex) {
	CHECK_HRESULT(commandAllocator_[allocatorIndex]->Reset());
	CHECK_HRESULT(commandList_->Reset(commandAllocator_[allocatorIndex].Get(), nullptr));
}

void DirectXDevice::CreateDevice() {
#ifdef _DEBUG	
	// デバッグ時のみ
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行えるようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	CHECK_HRESULT(CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf())));

	// 使用するアダプタ用の変数
	ComPtr<IDXGIAdapter4> useAdapter;

	// 良い順にアダプターを頼む
	for (uint32_t i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
		++i) {
		// アダプター情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		CHECK_HRESULT(useAdapter->GetDesc3(&adapterDesc));

		// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタ情報を出力
			Debug::Log(std::format(L"Use Adapter:{}\n", adapterDesc.Description));
			break;
		}
		useAdapter = nullptr; // ソフトウェアアダプタは見なかったことにする
	}
	assert(useAdapter != nullptr);


	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプターデバイスを生成
		HRESULT hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf()));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			Debug::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(device_ != nullptr);

#ifdef _DEBUG
	// デバッグ時のみ
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif
}

void DirectXDevice::CreateCommands() {
	// コマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	CHECK_HRESULT(device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf())));

	// コマンドアロケータを生成
	for (uint32_t i = 0; i < kSwapChainBufferCount; ++i) {
		CHECK_HRESULT(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_[i].GetAddressOf())));
	}

	// コマンドリストを生成
	CHECK_HRESULT(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_[0].Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf())));

	// フェンスを生成
	CHECK_HRESULT(device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf())));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXDevice::CreateDescriptorHeap() {
	rtvHeap_.Initalize(device_.Get(), kRTVDescriptorMaxCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, "RTV Descriptor Hrap");
	dsvHeap_.Initalize(device_.Get(), kDSVDescriptorMaxCount, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, "DSV Descriptor Hrap");
	commonHeap_.Initalize(device_.Get(), kCommonDescriptorMaxCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, "Common Descriptor Hrap");
}

void DirectXDevice::CreateSwapChain() {
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{
		.Width = swapChainWidth_,						// 画面幅
		.Height = swapChainHeight_,						// 画面高
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,			// 色の形式
		.SampleDesc{
			.Count = 1									// マルチサンプルしない
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,	// 描画ターゲットとして利用する
		.BufferCount = kSwapChainBufferCount,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,	// モニタに移したら、中身を破棄
	};

	ComPtr<IDXGISwapChain1> swapChain1;
	CHECK_HRESULT(dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		hwnd_,
		&swapChainDesc,
		nullptr,
		nullptr,
		swapChain1.GetAddressOf()));

	CHECK_HRESULT(swapChain1.As(&swapChain_));

	for (uint32_t i = 0; i < kSwapChainBufferCount; ++i) {
		CHECK_HRESULT(swapChain_->GetBuffer(i, IID_PPV_ARGS(swapChainResource_[i].GetAddressOf())));
		swapChainResource_[i]->SetName((L"SwapChainResource " + std::to_wstring(i)).c_str());
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
		};
		rtvHandle_[i] = rtvHeap_.Allocate();
		device_->CreateRenderTargetView(swapChainResource_[i].Get(), &rtvDesc, rtvHandle_[i].cpu);
	}
}

void DirectXDevice::CreateDepthStencilBuffer() {
	D3D12_RESOURCE_DESC resourceDesc{
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Width = swapChainWidth_,
		.Height = swapChainHeight_,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		.SampleDesc{
			.Count = 1
		},
		.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	};

	D3D12_HEAP_PROPERTIES heapProperties{
		.Type = D3D12_HEAP_TYPE_DEFAULT
	};

	D3D12_CLEAR_VALUE depthClearValue{
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		.DepthStencil{
			.Depth = 1.0f
		}
	};

	CHECK_HRESULT(device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(depthStencilResource_.GetAddressOf())));
	depthStencilResource_->SetName(L"DepthStencilBuffer");

	dsvHandle_ = dsvHeap_.Allocate();
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D
	};
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvHandle_.cpu);
}

void DirectXDevice::CreateImGui() {
	imguiDescriptor_ = commonHeap_.Allocate();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd_);
	ImGui_ImplDX12_Init(device_.Get(),
		kSwapChainBufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		commonHeap_.Get(),
		imguiDescriptor_.cpu,
		imguiDescriptor_.gpu);
}
