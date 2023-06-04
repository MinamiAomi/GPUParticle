#pragma once
#include "GPUResource.h"

namespace DirectXHelper {
	using namespace Microsoft::WRL;

	class RenderTarget :
		public GPUResource {
	public:
		bool Initalize(ID3D12Device5* device, ID3D12Resource* resource, DXGI_FORMAT viewFormat);
		bool Initalize(ID3D12Device5* device, uint32_t width, uint32_t height, DXGI_FORMAT format, float clearColor[4]);

		__declspec(property(get = GetHandle)) D3D12_CPU_DESCRIPTOR_HANDLE handle;
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetHandle() const { return handle_; }

		__declspec(property(get = GetWidth)) uint32_t width;
		inline uint32_t GetWidth() const { return width_; }

		__declspec(property(get = GetHeight)) uint32_t height;
		inline uint32_t GetHeight() const { return height_; }

		__declspec(property(get = GetFormat)) DXGI_FORMAT format;
		inline DXGI_FORMAT GetFormat() const { return format_; }

		__declspec(property(get = GetClearColor)) const float* clearColor;
		inline const float* GetClearColor() const { return clearColor_; }

	private:
		ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
		D3D12_CPU_DESCRIPTOR_HANDLE handle_{};
		uint32_t width_{ 0 };
		uint32_t height_{ 0 };
		DXGI_FORMAT format_{ DXGI_FORMAT_UNKNOWN };
		float clearColor_[4]{};
	};

}