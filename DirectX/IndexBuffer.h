#pragma once
#include "GPUResource.h"

namespace DirectXHelper {

	class IndexBuffer :
		public GPUResource {
	public:
		bool Initalize(ID3D12Device5* device, size_t indexCount);
		bool WriteData(const void* vertices);

		__declspec(property(get = GetView)) const D3D12_INDEX_BUFFER_VIEW& View;
		const D3D12_INDEX_BUFFER_VIEW& GetView() const { return view_; }

		__declspec(property(get = GetIndexCount)) uint32_t IndexCount;
		uint32_t GetIndexCount() const { return indexCount_; }

		__declspec(property(get = GetBufferSize)) uint32_t BufferSize;
		uint32_t GetBufferSize() const { return view_.SizeInBytes; }

		__declspec(property(get = GetFormat)) DXGI_FORMAT Format;
		DXGI_FORMAT GetFormat() const { return view_.Format; }

	private:
		D3D12_INDEX_BUFFER_VIEW view_{};
		uint32_t indexCount_{ 0 };
	};

}
