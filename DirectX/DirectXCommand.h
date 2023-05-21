#pragma once
#include "DirectXHelper.h"
#include "DirectXDevice.h"

class DirectXCommand
{
private:
	void Initalize(DirectXDevice& dxDevice);

	void SubmitCommandList();
	void WaitForGPU();
	void ResetCommandList(uint32_t backBufferIndex);
	
	ID3D12CommandQueue* GetCommandQueue() const { return cmdQueue_.Get(); }
	ID3D12GraphicsCommandList4* GetCommnadList() const { return cmdList_.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator(uint32_t backBufferIndex) const { return cmdAllocator_[backBufferIndex].Get(); }
	ID3D12Fence* GetFence() const { return fence_.Get(); }

public:
	DirectXHelper::ComPtr<ID3D12CommandQueue>			cmdQueue_;
	DirectXHelper::ComPtr<ID3D12GraphicsCommandList4>	cmdList_;
	DirectXHelper::ComPtr<ID3D12CommandAllocator>		cmdAllocator_[DirectXDevice::kSwapChainBufferCount];

	DirectXHelper::ComPtr<ID3D12Fence>					fence_;
	uint64_t											fenceValue_{ 0 };
	HANDLE												fenceEvent_{};
};

