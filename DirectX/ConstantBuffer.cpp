#include "stdafx.h"
#include "ConstantBuffer.h"
#include "Logger.h"

namespace DirectXHelper {
	ConstantBuffer::~ConstantBuffer() {
		if (mappedData_) {
			resource_->Unmap(0, nullptr);
			mappedData_ = nullptr;
		}
	}
	bool ConstantBuffer::Initalize(ID3D12Device5* device, size_t bufferSize) {
		assert(device);

		if (mappedData_) {
			resource_->Unmap(0, nullptr);
			mappedData_ = nullptr;
		}

		bufferSize_ = (static_cast<uint32_t>(bufferSize) + 0xFF - 1) & ~(0xFF - 1);
		if (!GPUResource::Initalize(
			device,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			CD3DX12_RESOURCE_DESC::Buffer(static_cast<uint64_t>(bufferSize_)),
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