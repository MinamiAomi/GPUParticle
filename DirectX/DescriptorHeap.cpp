#include "DescriptorHeap.h"
#include <cassert>
#include "StringUtils.h"

void DescriptorHeap::Initalize(ID3D12Device* device, uint32_t capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, bool isShaderVisible, const std::string& name) {
	assert(device);
	assert(capacity);
	assert(capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);

	if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
		isShaderVisible = false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc{
		.Type = type,
		.NumDescriptors = capacity,
		.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0
	};

	HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));
	heap_->SetName(ConvertString(name).c_str());

	cpuStartHandle_ = heap_->GetCPUDescriptorHandleForHeapStart();
	gpuStartHandle_ = isShaderVisible ? heap_->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(type);
	capacity_ = capacity;
	allocateIndex_ = 0;
	type_ = type;
}

DescriptorHandle DescriptorHeap::Allocate() {
	assert(heap_);
	assert(allocateIndex_ < capacity_);

	uint32_t offset = allocateIndex_ * descriptorSize_;
	DescriptorHandle result{};
	result.cpu.ptr = cpuStartHandle_.ptr + offset;
	if (IsShaderVisible()) {
		result.gpu.ptr = gpuStartHandle_.ptr + offset;
	}
#ifdef _DEBUG
	result.offset = offset;
	result.container = this;
#endif // _DEBUG
	++allocateIndex_;
	return result;
}

void DescriptorHeap::Deallocate(DescriptorHandle& descriptorHandle) {
	assert(descriptorHandle.IsValid());
	assert(heap_ && allocateIndex_);
	assert(descriptorHandle.container == this);
	assert(descriptorHandle.cpu.ptr >= cpuStartHandle_.ptr);
	assert((descriptorHandle.cpu.ptr - cpuStartHandle_.ptr) % descriptorSize_ == 0);
	assert(descriptorHandle.offset < capacity_);

	descriptorHandle = {};
}
