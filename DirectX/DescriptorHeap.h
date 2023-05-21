#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <string>

class DescriptorHeap;

struct DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpu{};

	bool IsValid() const { return cpu.ptr != 0; }
	bool IsShaderVisible() const { return gpu.ptr != 0; }
private:
#ifdef _DEBUG
	uint32_t offset{};
	DescriptorHeap* container{ nullptr };
	friend class DescriptorHeap;
#endif // _DEBUG
};

class DescriptorHeap {
public:
	void Initalize(
		ID3D12Device* device, 
		uint32_t capacity, 
		D3D12_DESCRIPTOR_HEAP_TYPE type, 
		bool isShaderVisible, 
		const std::string& name = "DescriptorHeap");
	
	[[nodiscard]] DescriptorHandle Allocate();
	void Deallocate(DescriptorHandle& descriptorHandle);

	ID3D12DescriptorHeap* Get() const { return heap_.Get(); }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetComPtr() const { return heap_; }
	uint32_t GetCapacity() const { return capacity_; }
	uint32_t GetSize() const { return allocateIndex_; }
	uint32_t GetDescriptorSize() const { return descriptorSize_; }
	D3D12_DESCRIPTOR_HEAP_TYPE GetType() const { return type_; }
	bool IsShaderVisible() const { return gpuStartHandle_.ptr != 0; }
	bool IsValid() const { return heap_; }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuStartHandle_{};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuStartHandle_{};
	uint32_t capacity_{};
	uint32_t allocateIndex_{};
	uint32_t descriptorSize_{};
	D3D12_DESCRIPTOR_HEAP_TYPE type_{};
};

