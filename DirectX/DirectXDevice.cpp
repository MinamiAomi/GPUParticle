#include "stdafx.h"
#include "DirectXDevice.h"
#include "Debug.h"

using namespace DirectXHelper;

void DirectXDevice::Initalize() {
	assert(!device_);
	assert(!commandQueue_);
	CreateDevice();
	CreateCommands();
}

void DirectXDevice::SubmitCommandList() {
	CHECK_HRESULT(commandList_->Close());
	ID3D12CommandList* cmdList[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, cmdList);
	++fenceValue_;
	CHECK_HRESULT(commandQueue_->Signal(fence_.Get(), fenceValue_));
}

void DirectXDevice::WaitForGPU() {
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
	for (uint32_t i = 0; i < kCommandAllocatorCount; ++i) {
		CHECK_HRESULT(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_[i].GetAddressOf())));
	}

	// コマンドリストを生成
	CHECK_HRESULT(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_[0].Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf())));

	// フェンスを生成
	CHECK_HRESULT(device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf())));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}
