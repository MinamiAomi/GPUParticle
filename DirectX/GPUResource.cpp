#include "stdafx.h"
#include "GPUResource.h"
#include "Logger.h"

namespace DirectXHelper {
	bool GPUResource::Initalize(
		ID3D12Device5* device,
		const D3D12_HEAP_PROPERTIES& heapProp,
		const D3D12_RESOURCE_DESC& desc,
		ResourceState initState,
		const D3D12_CLEAR_VALUE& clearValue) {
		assert(device);

		if (FAILED(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			static_cast<D3D12_RESOURCE_STATES>(initState),
			clearValue.Format != DXGI_FORMAT_UNKNOWN ? &clearValue : nullptr,
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())))) {
			Logger::Error("CreateCommittedResource()");
			assert(false);
			return false;
		}
		state_ = initState;
		return true;
	}
	D3D12_RESOURCE_BARRIER DirectXHelper::GPUResource::TransitionBarrier(ResourceState nextState) {
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = resource_.Get();
		barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(state_);
		barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(nextState);
		state_ = nextState;
		return barrier;
	}

	D3D12_RESOURCE_BARRIER GPUResource::UAVBarrier() {
		if (state_ != ResourceState::UnorderedAccess) {
			Logger::Warning("Resource state is not UnorderedAccess");
			assert(false);
		}
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.UAV.pResource = resource_.Get();
		return barrier;
	}

}