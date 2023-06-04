#pragma once

namespace Engine {
	using namespace Microsoft::WRL;

	class Graphics {
	public:
		static Graphics* GetInstance();

		bool Initalize();

		__declspec(property(get = GetDevice)) ID3D12Device5* Device;
		ID3D12Device5* GetDevice() { return device_.Get(); }

		__declspec(property(get = GetFactory)) IDXGIFactory6* Factory;
		IDXGIFactory6* GetFactory() { return factory_.Get(); }

	private:
		
		ComPtr<IDXGIFactory6> factory_;
		ComPtr<ID3D12Device5> device_;
		
	};

	static Graphics& graphics = *Graphics::GetInstance();
}
