#pragma once
#include "Fence.h"

namespace DirectXHelper {
	using namespace Microsoft::WRL;
		
	class CommandList {
	public:

		bool Initalize(ID3D12Device5* device, ID3D12CommandQueue* commandQueue);
		bool Execute();
		bool Wait();
		bool Reset();

		//void SetPipelineState(ID3D12PipelineState* pipelineState) {
		//	if (pipelineState_.Get() != pipelineState) {
		//		commandList_->SetPipelineState(pipelineState);
		//		resouceReferences_.emplace_back(pipelineState);
		//	}
		//}
		//void SetVertexBuffers(uint32_t viewCount, const D3D12_VERTEX_BUFFER_VIEW* views) {
		//	commandList_->IASetVertexBuffers(0, viewCount, views);
		//}
		//void SetIndexBuffers(const D3D12_INDEX_BUFFER_VIEW* view) {
		//	commandList_->IASetIndexBuffer(view);
		//}
		//void SetDescriptorHeaps(uint32_t heapCount, ID3D12DescriptorHeap* const heaps[]) {
		//	commandList_->SetDescriptorHeaps(heapCount, heaps);
		//}
		//void SetPrimitiveTopology();
		//void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature) {
		//	commandList_->SetGraphicsRootSignature(rootSignature);
		//}
		//void SetGraphicsRootConstantBufferView(uint32_t rootParameterIndex, ID3D12Resource* resource) {
		//	commandList_->SetGraphicsRootConstantBufferView(rootParameterIndex, resource->GetGPUVirtualAddress());
		//}
		//void SetGraphicsRootShaderResourceView(uint32_t rootParameterIndex, ID3D12Resource* resource) {
		//	commandList_->SetGraphicsRootShaderResourceView(rootParameterIndex, resource->GetGPUVirtualAddress());
		//}
		//void SetGraphicsRootDescriptorTable();
		//void SetComputeRootSignature();
		//void SetComputeRootConstantBufferView();
		//void SetComputeRootShaderResourceView();
		//void SetComputeRootDescriptorTable();
		//
		//void AddTransitionBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
		//	resouceReferences_.emplace_back(barrier);
		//	resouceReferences_.emplace_back(barrier.Transition.pResource);
		//}
		//void AddUAVBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
		//	resourceBarriers_.emplace_back(barrier);
		//	resouceReferences_.emplace_back(barrier.UAV.pResource);
		//}
		//void BatchResourceBarrier() {
		//	commandList_->ResourceBarrier(static_cast<uint32_t>(resourceBarriers_.size()), resourceBarriers_.data());
		//	resourceBarriers_.clear();
		//}
		//
		//void DrawInstanced();
		//void DrawIndexInstanced();
		//void Dispatch();

		bool IsEnabled() const { return commandList_; }
		ID3D12GraphicsCommandList4* Get() const { return commandList_.Get(); }

	private:
		Fence fence_;
		ComPtr<ID3D12GraphicsCommandList4> commandList_;
		ComPtr<ID3D12CommandAllocator> commandAllocator_;
		ComPtr<ID3D12CommandQueue> commandQueue_;

		bool isRecordingCommands = false;

		ComPtr<ID3D12PipelineState> pipelineState_;
		ComPtr<ID3D12RootSignature> rootSignature_;
		
		std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers_;

		std::vector<ComPtr<ID3D12Object>> resouceReferences_;
	};

}
