#pragma once

namespace DirectXHelper {
	using namespace Microsoft::WRL;

	class Fence {
	public:
		~Fence();
		bool Initalize(ID3D12Device* device);
		bool Signal(ID3D12CommandQueue* commandQueue);
		bool WaitForGPU();

	private:
		ComPtr<ID3D12Fence> fence_;
		HANDLE fenceEvent_{ nullptr };
		uint64_t fenceValue_{ 0 };
	};

}