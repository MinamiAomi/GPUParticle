#include "stdafx.h"
#include "Fence.h"
#include "Logger.h"

namespace DirectXHelper {

	Fence::~Fence() {
		if (fenceEvent_) {
			CloseHandle(fenceEvent_);
		}
	}

	bool Fence::Initalize(ID3D12Device* device) {
		assert(device);
		
		fenceValue_ = 0;
		if (FAILED(device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())))) {
			Logger::Error("CreateFence()");
			assert(false);
			return false;
		}
		fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!fenceEvent_) {
			Logger::Error("CreateHandle()");
			assert(false);
			return false;
		}
		return true;
	}

	bool Fence::Signal(ID3D12CommandQueue* commandQueue) {
		++fenceValue_;
		if (FAILED(commandQueue->Signal(fence_.Get(), fenceValue_))) {
			Logger::Error("ID3D12CommandQueue::Signal()");
			assert(false);
			return false;
		}
		return true;
	}

	bool Fence::WaitForGPU() {
		if (fence_->GetCompletedValue() < fenceValue_) {
			if (FAILED(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_))) {
				Logger::Error("ID3D12Fence::SetEventOnCompletion()");
				assert(false);
				return false;
			}
			WaitForSingleObject(fenceEvent_, INFINITE);
		}
		return true;
	}

}