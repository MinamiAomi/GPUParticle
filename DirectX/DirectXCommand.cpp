#include "DirectXCommand.h"

void DirectXCommand::Initalize(DirectXDevice& dxDevice) {
	auto device = dxDevice.GetDevice();
	// コマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	CHECK_SUCCESS(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(cmdQueue_.GetAddressOf())));

	// コマンドアロケータを生成
	for (uint32_t i = 0; i < _countof(cmdAllocator_); ++i) {
		CHECK_SUCCESS(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllocator_[i].GetAddressOf())));
	}

	// コマンドリストを生成
	CHECK_SUCCESS(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator_[0].Get(), nullptr, IID_PPV_ARGS(cmdList_.GetAddressOf())));

	// フェンスを生成
	CHECK_SUCCESS(device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf())));

	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommand::SubmitCommandList() {
	CHECK_SUCCESS(cmdList_->Close());
	ID3D12CommandList* cmdList[] = { cmdList_.Get() };
	cmdQueue_->ExecuteCommandLists(1, cmdList);
	++fenceValue_;
	CHECK_SUCCESS(cmdQueue_->Signal(fence_.Get(), fenceValue_));
}

void DirectXCommand::WaitForGPU() {
	if (fence_->GetCompletedValue() < fenceValue_) {
		CHECK_SUCCESS(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

void DirectXCommand::ResetCommandList(uint32_t backBufferIndex) {
	CHECK_SUCCESS(cmdAllocator_[backBufferIndex]->Reset());
	CHECK_SUCCESS(cmdList_->Reset(cmdAllocator_[backBufferIndex].Get(), nullptr));
}
