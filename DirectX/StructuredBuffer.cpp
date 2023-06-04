#include "stdafx.h"
#include "StructuredBuffer.h"
#include "Logger.h"

namespace DirectXHelper {

	bool StructuredBuffer::Initalize(ID3D12Device5* device, size_t elementCount, size_t elementSize) {
		assert(device);

		if (mappedData_) {
			resource_->Unmap(0, nullptr);
			mappedData_ = nullptr;
		}

		elementSize_ = static_cast<uint32_t>(elementSize);
		elementCount_ = static_cast<uint32_t>(elementCount);

		if (!GPUResource::Initalize(
			device,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			CD3DX12_RESOURCE_DESC::Buffer(static_cast<uint64_t>(BufferSize)),
			ResourceState::GenericRead)) {
			return false;
		}
		if (FAILED(resource_->Map(0, nullptr, &mappedData_))) {
			Logger::Error("ID3D12Resource::Map()");
			assert(false);
			return false;
		}
		return true;
	}

}