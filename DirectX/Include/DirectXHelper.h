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

	// Pretty-print a state object tree.
	void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc);
};

