#pragma once
namespace DirectXHelper {
	using namespace Microsoft::WRL;

	enum class ResourceState {
		Common = D3D12_RESOURCE_STATE_COMMON,
		VertexAndConstantBuffer = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		IndexBuffer = D3D12_RESOURCE_STATE_INDEX_BUFFER,
		RenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET,
		UnorderedAccess = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		DepthWrite = D3D12_RESOURCE_STATE_DEPTH_WRITE,
		DepthRead = D3D12_RESOURCE_STATE_DEPTH_READ,
		NonPixelShaderResource = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		PixelShaderResource = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		StreamOut = D3D12_RESOURCE_STATE_STREAM_OUT,
		IndirectArgument = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
		CopyDest = D3D12_RESOURCE_STATE_COPY_DEST,
		CopySource = D3D12_RESOURCE_STATE_COPY_SOURCE,
		ResolveDest = D3D12_RESOURCE_STATE_RESOLVE_DEST,
		ResolveSource = D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
		RaytracingAccelerationStructure = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		ShadingRateSource = D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE,
		GenericRead = D3D12_RESOURCE_STATE_GENERIC_READ,
		AllShaderResource = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		Present = D3D12_RESOURCE_STATE_PRESENT,
		Predication = D3D12_RESOURCE_STATE_PREDICATION,
		VideoDecodeRead = D3D12_RESOURCE_STATE_VIDEO_DECODE_READ,
		VideoDecodeWrite = D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE,
		VideoProcessRead = D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ,
		VideoProcessWrite = D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE,
		VideoEncodeRead = D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ,
		VideoEncodeWrite = D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE
	};

	class GPUResource {
	public:
		operator ID3D12Resource* () const { return resource_.Get(); }

		bool Initalize(
			ID3D12Device5* device,
			const D3D12_HEAP_PROPERTIES& heapProp,
			const D3D12_RESOURCE_DESC& desc,
			ResourceState initState,
			const D3D12_CLEAR_VALUE& colorValue = {});
		D3D12_RESOURCE_BARRIER TransitionBarrier(ResourceState nextState);
		D3D12_RESOURCE_BARRIER UAVBarrier();

		bool IsEnabled() const { return resource_; }
		ID3D12Resource* Get() const { return resource_.Get(); }
		ComPtr<ID3D12Resource> GetComPtr() const { return resource_; }
		ID3D12Resource* const* GetAddressOf() const { return resource_.GetAddressOf(); }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return resource_->GetGPUVirtualAddress(); }

		__declspec(property(get = GetState)) ResourceState State;
		ResourceState GetState() const { return state_; }

	protected:
		ComPtr<ID3D12Resource> resource_;
		ResourceState state_{ ResourceState::Common };
	};

}