#pragma once
#include "GPUResource.h"

namespace DirectXHelper {

	class VertexBuffer :
		public GPUResource {
	public:
		bool Initalize(ID3D12Device5* device, size_t vertexCount, size_t strideSize);
		bool WriteData(const void* vertices);

		__declspec(property(get = GetView)) const D3D12_VERTEX_BUFFER_VIEW& View;
		const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return view_; }

		__declspec(property(get = GetVertexCount)) uint32_t VertexCount;
		uint32_t GetVertexCount() const { return vertexCount_; }

		__declspec(property(get = GetBufferSize)) uint32_t BufferSize;
		uint32_t GetBufferSize() const { return view_.SizeInBytes; }

		__declspec(property(get = GetStrideSize)) uint32_t StrideSize;
		uint32_t GetStrideSize() const { return view_.StrideInBytes; }

	private:
		D3D12_VERTEX_BUFFER_VIEW view_{};
		uint32_t vertexCount_{ 0 };

	};

}
