#include "stdafx.h"
#include "RenderTarget.h"
#include "Logger.h"

namespace DirectXHelper {
	bool RenderTarget::Initalize(ID3D12Device5* device, ID3D12Resource* resource, DXGI_FORMAT viewFormat) {
		assert(device);
		assert(resource);

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.NumDescriptors = 1;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		if (FAILED(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf())))) {
			Logger::Error("CreateDescriptorHeap()");
			assert(false);
			return false;
		}
		handle_ = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = viewFormat;
		device->CreateRenderTargetView(resource, &rtvDesc, handle_);

		resource_.Attach(resource);
		auto resourceDesc = resource_->GetDesc();
		width_ = static_cast<uint32_t>(resourceDesc.Width);
		height_ = resourceDesc.Height;
		format_ = viewFormat;

		return true;
	}

	bool RenderTarget::Initalize(ID3D12Device5* device, uint32_t width, uint32_t height, DXGI_FORMAT format, float clearColor[4]) {
		assert(device);
		assert(width > 0);
		assert(height > 0);
		assert(format != DXGI_FORMAT_UNKNOWN);

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		auto clearValue = CD3DX12_CLEAR_VALUE(format, clearColor);
		memcpy(clearColor_, clearColor, sizeof(clearColor_));

		ID3D12Resource* tmpResource = nullptr;
		if (FAILED(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PRESENT,
			&clearValue,
			IID_PPV_ARGS(&tmpResource)))) {
			Logger::Error("CreateCommittedResource()");
			assert(false);
			return false;
		}

		return Initalize(device, tmpResource,format);
	}

}