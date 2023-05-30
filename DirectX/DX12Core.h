#pragma once

namespace DirectXHelper {
	class Core {
	public:
		static bool Initalize();
		static IDXGIFactory6* GetFactory() { return GetInstance()->factory_.Get(); }
		static IDXGIAdapter4* GetAdapter() { return GetInstance()->adapter_.Get(); }
		static ID3D12Device5* GetDevice() { return GetInstance()->device_.Get(); }

	private:
		static Core* GetInstance();
		bool InternalInitalize();

		Microsoft::WRL::ComPtr<IDXGIFactory6> factory_;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter_;
		Microsoft::WRL::ComPtr<ID3D12Device5> device_;
	};
}
