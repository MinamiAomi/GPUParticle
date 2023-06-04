#include "stdafx.h"
#include "IndexBuffer.h"
#include "Logger.h"

namespace DirectXHelper {

	bool IndexBuffer::Initalize(ID3D12Device5* device, size_t indexCount) {
		assert(device);

		indexCount_ = static_cast<uint32_t>(indexCount);
		view_.Format = DXGI_FORMAT_R16_UINT;
		view_.SizeInBytes = indexCount_ * sizeof(uint16_t);

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

	bool IndexBuffer::WriteData(const void* indices) {
		assert(IsEnabled());
		void* mappedData = nullptr;
		if (FAILED(resource_->Map(0, nullptr, &mappedData))) {
			Logger::Error("ID3D12Resource::Map()");
			assert(false);
			return false;
		}
		memcpy(mappedData, indices, view_.SizeInBytes);
		resource_->Unmap(0, nullptr);
		mappedData = nullptr;
		return true;
	}

}