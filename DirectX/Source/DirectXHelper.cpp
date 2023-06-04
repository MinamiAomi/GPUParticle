#include "DirectXHelper.h"
#include "StringUtils.h"
#include "Debug.h"
#include "VertexBuffer.h"

namespace DirectXHelper {

	ComPtr<ID3D12Resource> CreateBuffer(ID3D12Device* device, uint64_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState, D3D12_HEAP_TYPE heapType) {
		auto heapProps = CD3DX12_HEAP_PROPERTIES(heapType);
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size, flags);

		ComPtr<ID3D12Resource> buffer;
		CHECK_HRESULT(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			initState,
			nullptr, IID_PPV_ARGS(buffer.GetAddressOf())));

		return buffer;
	}

	ComPtr<ID3D12Resource> CreateBufferUAV(ID3D12Device* device, uint64_t size, D3D12_RESOURCE_STATES initState) {
		return CreateBuffer(device, size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, initState, D3D12_HEAP_TYPE_DEFAULT);
	}

	void WriteToBuffer(ID3D12Resource* buffer, size_t byteSize, void* source) {
		void* destination{ nullptr };
		CHECK_HRESULT(buffer->Map(0, nullptr, &destination));
		memset(destination, 0, buffer->GetDesc().Width);
		memcpy(destination, source, byteSize);
		buffer->Unmap(0, nullptr);
	}

	uint64_t Align(uint64_t value, uint64_t alignment) {
		return (value + alignment - 1) & ~(alignment - 1);
	};
	uint32_t Align(uint32_t value, uint32_t alignment) {
		return (value + alignment - 1) & ~(alignment - 1);
	};

#pragma region ディスクリプタヒープ
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

		CHECK_HRESULT(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap_.ReleaseAndGetAddressOf())));
		heap_->SetName(String::Convert(name).c_str());

		cpuStartHandle_ = heap_->GetCPUDescriptorHandleForHeapStart();
		gpuStartHandle_ = isShaderVisible ? heap_->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
		descriptorSize_ = device->GetDescriptorHandleIncrementSize(type);
		capacity_ = capacity;
		allocateIndex_ = 0;
		type_ = type;
	}

	Descriptor DescriptorHeap::Allocate() {
		assert(heap_);
		assert(allocateIndex_ < capacity_);

		uint32_t offset = allocateIndex_ * descriptorSize_;
		Descriptor result{};
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

	void DescriptorHeap::Deallocate(Descriptor& descriptorHandle) {
		assert(descriptorHandle.IsEnabled());
		assert(heap_ && allocateIndex_);
		assert(descriptorHandle.container == this);
		assert(descriptorHandle.cpu.ptr >= cpuStartHandle_.ptr);
		assert((descriptorHandle.cpu.ptr - cpuStartHandle_.ptr) % descriptorSize_ == 0);
		assert(descriptorHandle.offset < capacity_);

		descriptorHandle = {};
	}
#pragma endregion ディスクリプタヒープ


};