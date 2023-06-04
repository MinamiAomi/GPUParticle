#include "stdafx.h"
#include "VertexBuffer.h"
#include "Logger.h"

namespace DirectXHelper {

	bool VertexBuffer::Initalize(ID3D12Device5* device, size_t vertexCount, size_t strideSize) {
		assert(device);

		vertexCount_ = static_cast<uint32_t>(vertexCount);
		view_.StrideInBytes = static_cast<uint32_t>(strideSize);
		view_.SizeInBytes = vertexCount_ * view_.StrideInBytes;

		if (!GPUResource::Initalize(
			device,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			CD3DX12_RESOURCE_DESC::Buffer(view_.SizeInBytes),
			ResourceState::GenericRead)) {
			return false;
		}
		view_.BufferLocation = GPUResource::GetGPUAddress();
		return true;
	}

	bool VertexBuffer::WriteData(const void* vertices) {
		assert(IsEnabled());
		void* mappedData = nullptr;
		if (FAILED(resource_->Map(0, nullptr, &mappedData))) {
			Logger::Error("ID3D12Resource::Map()");
			assert(false);
			return false;
		}
		memcpy(mappedData, vertices, view_.SizeInBytes);
		resource_->Unmap(0, nullptr);
		mappedData = nullptr;
		return true;
	}

}