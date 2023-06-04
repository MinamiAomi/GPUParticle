#pragma once

#define CHECK_HRESULT(hr) { if(FAILED(hr)) assert(false); }

namespace DirectXHelper {
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// バッファを生成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="size">バッファサイズ</param>
	/// <param name="flags">バッファフラグ</param>
	/// <param name="initState">初期化状態</param>
	/// <param name="heapProps">ヒープ設定</param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBuffer(
		ID3D12Device* device,
		uint64_t size,
		D3D12_RESOURCE_FLAGS flags,
		D3D12_RESOURCE_STATES initState,
		D3D12_HEAP_TYPE heapType);
	/// <summary>
	/// UAVバッファを生成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="size">バッファサイズ</param>
	/// <param name="initState">初期化状態</param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateBufferUAV(
		ID3D12Device* device,
		uint64_t size,
		D3D12_RESOURCE_STATES initState);

	/// <summary>
	/// バッファに書き込む
	/// </summary>
	/// <param name="buffer">バッファ</param>
	/// <param name="byteSize">書き込むサイズ</param>
	/// <param name="source">書き込みデータ</param>
	void WriteToBuffer(ID3D12Resource* buffer, size_t byteSize, void* source);

	uint64_t Align(uint64_t value, uint64_t alignment);
	uint32_t Align(uint32_t value, uint32_t alignment);


#pragma region ディスクリプタヒープ
	class DescriptorHeap;

	struct Descriptor {
		D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
		D3D12_GPU_DESCRIPTOR_HANDLE gpu{};

		bool IsEnabled() const { return cpu.ptr != 0; }
		bool IsShaderVisible() const { return gpu.ptr != 0; }
	private:
#ifdef _DEBUG
		uint32_t offset{};
		DescriptorHeap* container{ nullptr };
		friend class DescriptorHeap;
#endif // _DEBUG
	};

	class DescriptorHeap {
	public:
		DescriptorHeap() = default;
		DELETE_COPY_MOVE(DescriptorHeap);
		void Initalize(
			ID3D12Device* device,
			uint32_t capacity,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			bool isShaderVisible,
			const std::string& name = "DescriptorHeap");

		[[nodiscard]] Descriptor Allocate();
		void Deallocate(Descriptor& descriptorHandle);

		ID3D12DescriptorHeap* Get() const { return heap_.Get(); }
		DirectXHelper::ComPtr<ID3D12DescriptorHeap> GetComPtr() const { return heap_; }
		uint32_t GetCapacity() const { return capacity_; }
		uint32_t GetSize() const { return allocateIndex_; }
		uint32_t GetDescriptorSize() const { return descriptorSize_; }
		D3D12_DESCRIPTOR_HEAP_TYPE GetType() const { return type_; }
		bool IsShaderVisible() const { return gpuStartHandle_.ptr != 0; }
		bool IsEnabled() const { return heap_; }

	private:
		DirectXHelper::ComPtr<ID3D12DescriptorHeap> heap_;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuStartHandle_{};
		D3D12_GPU_DESCRIPTOR_HANDLE gpuStartHandle_{};
		uint32_t capacity_{};
		uint32_t allocateIndex_{};
		uint32_t descriptorSize_{};
		D3D12_DESCRIPTOR_HEAP_TYPE type_{};
	};
#pragma endregion ディスクリプタヒープ

};

#define ASSERT_HRESULT(hr) { if(FAILED(hr)) assert(false); }
