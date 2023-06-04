#pragma once
#include "GPUResource.h"

namespace DirectXHelper {

	class StructuredBuffer :
		public GPUResource {
	public:
		bool Initalize(ID3D12Device5* device, size_t elementCount, size_t elementSize);

		__declspec(property(get = GetMappedData)) void* MappedData;
		void* GetMappedData() const { return mappedData_; }

		__declspec(property(get = GetBufferSize)) uint32_t BufferSize;
		uint32_t GetBufferSize() const { return elementCount_ * elementSize_; }

		__declspec(property(get = GetElementCount)) uint32_t ElementCount;
		uint32_t GetElementCount() const { return elementCount_; }

		__declspec(property(get = GetElementSize)) uint32_t ElementSize;
		uint32_t GetElementSize() const { return elementSize_; }

	private:
		void* mappedData_{ nullptr };
		uint32_t elementCount_{ 0 };
		uint32_t elementSize_{ 0 };
	};

}