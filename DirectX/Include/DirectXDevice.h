#pragma once
#include "DirectXHelper.h"

class DirectXDevice {
public:
	DirectXDevice() = default;
	DELETE_COPY_MOVE(DirectXDevice);

	static const uint32_t kSwapChainBufferCount = 2;
	static const uint32_t kRTVDescriptorMaxCount = 16;
	static const uint32_t kDSVDescriptorMaxCount = 8;
	static const uint32_t kCommonDescriptorMaxCount = 1024;


	void Initalize(HWND hwnd_, uint32_t clientWidth, uint32_t clientHeight);
	void Finalize();
	void StertScreenRendering();
	void FinishScreenRendering();

	void SubmitCommandList();
	void WaitForGPU();
	void ResetCommandList(uint32_t allocatorIndex);

	IDXGIFactory7* GetDXGIFactory() const { return dxgiFactory_.Get(); }
	ID3D12Device5* GetDevice() const { return device_.Get(); }
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }
	ID3D12GraphicsCommandList4* GetCommnadList() const { return commandList_.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator(uint32_t index) const { return commandAllocator_[index].Get(); }
	ID3D12Fence* GetFence() const { return fence_.Get(); }
	DirectXHelper::DescriptorHeap& GetRTVHeap() { return rtvHeap_; }
	DirectXHelper::DescriptorHeap& GetDSVHeap() { return dsvHeap_; }
	DirectXHelper::DescriptorHeap& GetCommonHeap() { return commonHeap_; }
	IDXGISwapChain4* GetSwapChain() const { return swapChain_.Get(); }
	ID3D12Resource* GetSwapChianResource(uint32_t index) const { return swapChainResource_[index].Get(); }
	const DirectXHelper::Descriptor& GetRTVHandle(uint32_t index) const { return rtvHandle_[index]; }
	ID3D12Resource* GetDepthStencilResource() const { return depthStencilResource_.Get(); }
	const DirectXHelper::Descriptor& GetDSVHandle() const { return dsvHandle_; }
	uint32_t GetSwapChainWidth() const { return swapChainWidth_; }
	uint32_t GetSwapChainHeight() const { return swapChainHeight_; }
	
private:
	void CreateDevice();
	void CreateCommands();
	void CreateDescriptorHeap();
	void CreateSwapChain();
	void CreateDepthStencilBuffer();

	HWND												hwnd_{};

	DirectXHelper::ComPtr<ID3D12Device5>				device_;
	DirectXHelper::ComPtr<IDXGIFactory7>				dxgiFactory_;

	DirectXHelper::ComPtr<ID3D12CommandQueue>			commandQueue_;
	DirectXHelper::ComPtr<ID3D12GraphicsCommandList4>	commandList_;
	DirectXHelper::ComPtr<ID3D12CommandAllocator>		commandAllocator_[kSwapChainBufferCount];

	DirectXHelper::ComPtr<ID3D12Fence>					fence_;
	uint64_t											fenceValue_{ 0 };
	HANDLE												fenceEvent_{};

	DirectXHelper::DescriptorHeap						rtvHeap_;
	DirectXHelper::DescriptorHeap						dsvHeap_;
	DirectXHelper::DescriptorHeap						commonHeap_;

	DirectXHelper::ComPtr<IDXGISwapChain4>				swapChain_;
	DirectXHelper::ComPtr<ID3D12Resource>				swapChainResource_[kSwapChainBufferCount]{};
	DirectXHelper::Descriptor							rtvHandle_[kSwapChainBufferCount];
	DirectXHelper::ComPtr<ID3D12Resource>				depthStencilResource_;
	DirectXHelper::Descriptor							dsvHandle_{};
	uint32_t											swapChainWidth_{ 0 };
	uint32_t											swapChainHeight_{ 0 };
};

