#pragma once
#include "DirectXHelper.h"
#include "DescriptorHeap.h"

class DirectXDevice {
public:
	static const uint32_t kSwapChainBufferCount = 2;
	static const uint32_t kRTVDescriptorMaxCount = 16;
	static const uint32_t kDSVDescriptorMaxCount = 8;
	static const uint32_t kCommonDescriptorMaxCount = 1024;

	void Initalize(HWND hwnd, uint32_t width, uint32_t height);

	void SubmitCommandList();
	void WaitForGPU();
	void ResetCommandList(uint32_t backBufferIndex);

	[[nodiscard]] DescriptorHandle Allocate(D3D12_DESCRIPTOR_HEAP_TYPE);
	void Deallocate(DescriptorHandle& descriptorHandle);

	IDXGIFactory7* GetDXGIFactory() const { return dxgiFactory_.Get(); }
	ID3D12Device5* GetDevice() const { return device_.Get(); }
	ID3D12CommandQueue* GetCommandQueue() const { return cmdQueue_.Get(); }
	ID3D12GraphicsCommandList4* GetCommnadList() const { return cmdList_.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator(uint32_t backBufferIndex) const { return cmdAllocator_[backBufferIndex].Get(); }
	ID3D12Fence* GetFence() const { return fence_.Get(); }

private:
	void CreateDevice();
	void CreateCommand();
	void CreateDescriptorHeap();
	void CreateRenderObject();
	void InitalizeImGui();

	HWND												hwnd_{};

	DirectXHelper::ComPtr<IDXGIFactory7>				dxgiFactory_;
	DirectXHelper::ComPtr<ID3D12Device5>				device_;

	DirectXHelper::ComPtr<ID3D12CommandQueue>			cmdQueue_;
	DirectXHelper::ComPtr<ID3D12GraphicsCommandList4>	cmdList_;
	DirectXHelper::ComPtr<ID3D12CommandAllocator>		cmdAllocator_[kSwapChainBufferCount];

	DirectXHelper::ComPtr<ID3D12Fence>					fence_;
	uint64_t											fenceValue_{ 0 };
	HANDLE												fenceEvent_{};

	DescriptorHeap										rtvHeap_;
	DescriptorHeap										dsvHeap_;
	DescriptorHeap										commonHeap_;

	DirectXHelper::ComPtr<IDXGISwapChain4>				swapChain_;
	DirectXHelper::ComPtr<ID3D12Resource>				swapChainResource_[kSwapChainBufferCount]{};
	DescriptorHandle									rtvHandle_[kSwapChainBufferCount];
	DirectXHelper::ComPtr<ID3D12Resource>				depthStencilResource_;
	DescriptorHandle									dsvHandle_{};
	uint32_t											swapChainWidth_{ 0 };
	uint32_t											swapChainHeight_{ 0 };
};

