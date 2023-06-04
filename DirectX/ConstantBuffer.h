#pragma once
#include "GPUResource.h"

namespace DirectXHelper {

	class ConstantBuffer : 
		public GPUResource {
	public:
		~ConstantBuffer();

		bool Initalize(ID3D12Device5* device, size_t bufferSize);

		__declspec(property(get = GetMappedData)) void* MappedData;
		void* GetMappedData() const { return mappedData_; }
		
		__declspec(property(get = GetBufferSize)) uint32_t BufferSize;
		uint32_t GetBufferSize() const { return bufferSize_; }

	private:
		uint32_t bufferSize_{ 0 };
		void* mappedData_{ nullptr };
	};

}