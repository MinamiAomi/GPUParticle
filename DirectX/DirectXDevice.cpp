#include "DirectXDevice.h"
#include "Debug.h"
#include <format>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

using namespace DirectXHelper;

void DirectXDevice::Initalize(HWND hwnd, uint32_t width, uint32_t height) {
	hwnd_ = hwnd;
	swapChainWidth_ = width;
	swapChainHeight_ = height;
	
	CreateDevice();
	CreateCommand();
	CreateDescriptorHeap();
	CreateRenderObject();
}

void DirectXDevice::SubmitCommandList() {
	CHECK_SUCCESS(cmdList_->Close());
	ID3D12CommandList* cmdList[] = { cmdList_.Get() };
	cmdQueue_->ExecuteCommandLists(1, cmdList);
	++fenceValue_;
	CHECK_SUCCESS(cmdQueue_->Signal(fence_.Get(), fenceValue_));
}

void DirectXDevice::WaitForGPU() {
	if (fence_->GetCompletedValue() < fenceValue_) {
		CHECK_SUCCESS(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

void DirectXDevice::ResetCommandList(uint32_t backBufferIndex) {
	CHECK_SUCCESS(cmdAllocator_[backBufferIndex]->Reset());
	CHECK_SUCCESS(cmdList_->Reset(cmdAllocator_[backBufferIndex].Get(), nullptr));
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

	CHECK_SUCCESS(CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf())));

	// 使用するアダプタ用の変数
	ComPtr<IDXGIAdapter4> useAdapter;

	// 良い順にアダプターを頼む
	for (uint32_t i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
		++i) {
		// アダプター情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		CHECK_SUCCESS(useAdapter->GetDesc3(&adapterDesc));

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

void DirectXDevice::CreateCommand() {
	// コマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	CHECK_SUCCESS(device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(cmdQueue_.GetAddressOf())));

	// コマンドアロケータを生成
	for (uint32_t i = 0; i < kSwapChainBufferCount; ++i) {
		CHECK_SUCCESS(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllocator_[i].GetAddressOf())));
	}

	// コマンドリストを生成
	CHECK_SUCCESS(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator_[0].Get(), nullptr, IID_PPV_ARGS(cmdList_.GetAddressOf())));

	// フェンスを生成
	CHECK_SUCCESS(device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf())));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXDevice::CreateDescriptorHeap() {
	rtvHeap_.Initalize(device_.Get(), kRTVDescriptorMaxCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false, "RTV Descriptor Hrap");
	dsvHeap_.Initalize(device_.Get(), kDSVDescriptorMaxCount, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, "DSV Descriptor Hrap");
	commonHeap_.Initalize(device_.Get(), kCommonDescriptorMaxCount, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, "Common Descriptor Hrap");
}

void DirectXDevice::CreateRenderObject() {
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = swapChainWidth_;		// 画面幅
	swapChainDesc.Height = swapChainHeight_;	// 画面高
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
	swapChainDesc.SampleDesc.Count = 1;					// マルチサンプル市内
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画ターゲットとして利用する
	swapChainDesc.BufferCount = kSwapChainBufferCount;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタに移したら、中身を破棄

	ComPtr<IDXGISwapChain1> swapChain1;
	CHECK_SUCCESS(dxgiFactory_->CreateSwapChainForHwnd(
		cmdQueue_.Get(),
		hwnd_, 
		&swapChainDesc,
		nullptr, 
		nullptr, 
		swapChain1.GetAddressOf()));

	CHECK_SUCCESS(swapChain1.As(&swapChain_));

	for (uint32_t i = 0; i < kSwapChainBufferCount; ++i) {
		CHECK_SUCCESS(swapChain_->GetBuffer(i, IID_PPV_ARGS(swapChainResource_[i].GetAddressOf())));
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvHandle_[i] = rtvHeap_.Allocate();
		device_->CreateRenderTargetView(swapChainResource_[i].Get(), &rtvDesc, rtvHandle_[i].cpu);
	}

}

void DirectXDevice::InitalizeImGui() {
	auto descriptor = commonHeap_.Allocate();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd_);
	ImGui_ImplDX12_Init(device_.Get(),
		kSwapChainBufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		commonHeap_.Get(),
		descriptor.cpu,
		descriptor.gpu);
}
