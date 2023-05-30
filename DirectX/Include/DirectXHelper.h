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
#pragma region ルートシグネチャ
	class RootSignatureDesc {
	public:
		enum class DescriptorType {
			CBV = D3D12_ROOT_PARAMETER_TYPE_CBV,
			SRV = D3D12_ROOT_PARAMETER_TYPE_SRV,
			UAV = D3D12_ROOT_PARAMETER_TYPE_UAV
		};

		enum class RangeType {
			SRV = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			UAV = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
			CBV = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER
		};

		enum class AddressMode {
			Wrap = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			Mirror = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
			Clamp = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			Border = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			MirrorOnce = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
		};

		void AddDescriptorTable();
		void AddDescriptor(DescriptorType descType, uint32_t shaderRegister, uint32_t registerSpace = 0);
		void AddDescriptorRange(
			RangeType rangeType,
			uint32_t numDescriptors,
			uint32_t baseShaderRegister,
			uint32_t registerSpace = 0,
			uint32_t offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
		void AddStaticSampler(
			uint32_t shaderRegister,
			uint32_t ragisterSpace = 0,
			D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			AddressMode addressU = AddressMode::Wrap,
			AddressMode addressV = AddressMode::Wrap,
			AddressMode addressW = AddressMode::Wrap);
		void AddFlags(D3D12_ROOT_SIGNATURE_FLAGS flags);
		void Clear();

	private:
		std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> ranges_;
		std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescs_;
		D3D12_ROOT_SIGNATURE_FLAGS flags_ = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		friend class RootSignature;
	};

	class RootSignature {
	public:
		RootSignature() = default;
		DELETE_COPY_MOVE(RootSignature);

		void Create(ID3D12Device* device, RootSignatureDesc& rootSignatureDesc, const std::string& name = "RootSignature");

		ID3D12RootSignature* Get() const { return rootSignature_.Get(); }
		ComPtr<ID3D12RootSignature> GetComPtr() const { return rootSignature_; }
		ID3D12RootSignature* const* GetAddressOf() const { return rootSignature_.GetAddressOf(); }

	private:
		ComPtr<ID3D12RootSignature> rootSignature_;
	};
#pragma endregion ルートシグネチャ
#pragma region パイプライン

	class GraphicsPipelineStateDesc {
	public:
		// ブレンドモード
		enum class BlendMode {
			None,		// 合成なし
			Normal,		// 通常
			Add,		// 加算合成
			Subtract,	// 減算合成
			Multiply,	// 乗算合成
			Inverse		// 反転
		};
		// フィルモード
		enum class FillMode {
			WireFrame = D3D12_FILL_MODE_WIREFRAME,	// ワイヤーフレーム
			Solid = D3D12_FILL_MODE_SOLID			// ソリッド
		};
		// カリングモード
		enum class CullMode {
			None = D3D12_CULL_MODE_NONE,	// カリングしない
			Front = D3D12_CULL_MODE_FRONT,	// 前面カリング
			Back = D3D12_CULL_MODE_BACK,	// 背面カリング
		};
		// 深度書き込みマスク
		enum class DepthWriteMask {
			Zero = D3D12_DEPTH_WRITE_MASK_ZERO,		// 書き込みをオフ
			All = D3D12_DEPTH_WRITE_MASK_ALL		// 書き込みをオン
		};
		// 比較関数
		enum class ComparisonFunc {
			Never = D3D12_COMPARISON_FUNC_NEVER,				// 比較に成功しない
			Less = D3D12_COMPARISON_FUNC_LESS,					// 小さい場合
			Equal = D3D12_COMPARISON_FUNC_EQUAL,				// 等しい場合
			LessEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,		// 以下の場合
			Greater = D3D12_COMPARISON_FUNC_GREATER,			// より大きい場合
			NotEqual = D3D12_COMPARISON_FUNC_NOT_EQUAL,			// 等しくない場合
			GreaterEqual = D3D12_COMPARISON_FUNC_GREATER_EQUAL,	// 以上の場合
			Always = D3D12_COMPARISON_FUNC_ALWAYS				// 比較を渡す
		};
		// プリミティブ型
		enum class PrimitiveTopology {
			Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,		// 点
			Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,			// 線
			Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE	// 三角形
		};

		void SetRootSignature(ID3D12RootSignature* rootSignature);
		void SetVertexShader(const void* shaderBytecode, SIZE_T bytecodeLength);
		void SetPixelShader(const void* shaderBytecode, SIZE_T bytecodeLength);
		void SetDomainShader(const void* shaderBytecode, SIZE_T bytecodeLength);
		void SetHullShader(const void* shaderBytecode, SIZE_T bytecodeLength);
		void SetGeometryShader(const void* shaderBytecode, SIZE_T bytecodeLength);
		void SetRasterizerState(FillMode fillMode, CullMode cullMode);
		void AddInputElementVertex(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot);
		void AddInputElementInstance(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot, uint32_t instanceDataStepRate);
		void SetPrimitiveTopologyType(PrimitiveTopology primitiveTopologyType);
		void AddRenderTargetState(BlendMode blendMode, DXGI_FORMAT rtvFormat);
		void SetDepthState(DepthWriteMask depthWriteMask, ComparisonFunc comparisonFunc, DXGI_FORMAT dsvFormat);
		void SetSampleState(uint32_t count, uint32_t quality);

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_{};
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements_;
		uint32_t renderTargetCount_{ 0 };
		std::vector<std::string> semanticNames;

		friend class PipelineState;
	};

	class ComputePipelineStateDesc {
	public:
		void SetRootSignature(ID3D12RootSignature* rootSignature);
		void SeComputeShader(const void* shaderBytecode, SIZE_T bytecodeLength);

	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc_{};

		friend class PipelineState;
	};

	class PipelineState {
	public:
		PipelineState() = default;
		DELETE_COPY_MOVE(PipelineState);

		void Create(ID3D12Device* device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::string& name = "GraphicsPipelineState");
		void Create(ID3D12Device* device, GraphicsPipelineStateDesc& desc, const std::string& name = "GraphicsPipelineState");
		void Create(ID3D12Device* device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc, const std::string& name = "ComputePipelineState");
		void Create(ID3D12Device* device, ComputePipelineStateDesc& desc, const std::string& name = "ComputePipelineState");


		ID3D12PipelineState* Get() const { return pipelineState_.Get(); }
		ComPtr<ID3D12PipelineState> GetComPtr() const { return pipelineState_; }
		ID3D12PipelineState* const* GetAddressOf() const { return pipelineState_.GetAddressOf(); }
	private:
		ComPtr<ID3D12PipelineState> pipelineState_;
	};

#pragma endregion パイプライン
#pragma region GPUリソース
	class GPUResource {
	public:
		GPUResource() = default;
		DELETE_COPY_MOVE(GPUResource);
		void Create(ID3D12Device* device,
			const D3D12_HEAP_PROPERTIES& heapProperties,
			const D3D12_RESOURCE_DESC& resourceDesc,
			D3D12_RESOURCE_STATES initialResourceState,
			const std::string& name);

		bool IsEnabled() const { return gpuResource_; }
		ID3D12Resource* Get() const { return gpuResource_.Get(); }
		ComPtr<ID3D12Resource> GetComPtr() const { return gpuResource_; }
		D3D12_GPU_VIRTUAL_ADDRESS  GetGPUAddress() const { return gpuResource_->GetGPUVirtualAddress(); }

	protected:
		ComPtr<ID3D12Resource> gpuResource_;
	};
#pragma endregion GPUリソース
#pragma region 頂点バッファ
	class VertexBuffer : public GPUResource {
	public:
		VertexBuffer() = default;
		DELETE_COPY_MOVE(VertexBuffer);
		/// <summary>
		/// <para> 頂点バッファを生成 </para>
		/// <para> すでに生成している場合は再生成 </para>
		/// </summary>
		/// <param name="device">デバイス</param>
		/// <param name="vertexCount">頂点数</param>
		/// <param name="strideSize">頂点のバイトサイズ</param>
		/// <param name="name">頂点バッファの名前（デバック用）</param>
		void Create(ID3D12Device* device, uint32_t vertexCount, uint32_t strideSize, const std::string& name = "VertexBuffer");
		/// <summary>
		/// 頂点を書き込む
		/// </summary>
		/// <param name="vertices">頂点</param>
		void WriteData(const void* vertices);
		/// <summary>
		/// 頂点を書き込む
		/// </summary>
		/// <typeparam name="VERTEX_TYPE">頂点の型</typeparam>
		/// <param name="vertices">頂点</param>
		template<class VERTEX_TYPE>
		void WriteData(const std::vector<VERTEX_TYPE>& vertices) {
			WriteData(static_cast<const void*>(vertices.data()));
		}

		uint32_t GetBufferSize() const { return  vertexCount_ * strideSize_; }
		uint32_t GetStrideSize() const { return  strideSize_; }
		uint32_t GetVertexCount() const { return vertexCount_; }
		D3D12_VERTEX_BUFFER_VIEW GetView() const { return { GetGPUAddress(), GetBufferSize(), strideSize_ }; }

	private:
		uint32_t vertexCount_{ 0 };
		uint32_t strideSize_{ 0 };
	};
#pragma endregion 頂点バッファ
#pragma region インデックスバッファ
	using Index = uint16_t;
	class IndexBuffer : public GPUResource {
	public:
		static const uint32_t kStrideSize = sizeof(Index);

		IndexBuffer() = default;
		DELETE_COPY_MOVE(IndexBuffer);
		/// <summary>
		/// <para> インデックスバッファを生成 </para>
		/// <para> すでに生成している場合は再生成 </para>
		/// </summary>
		/// <param name="device">デバイス</param>
		/// <param name="indexCount">インデックス数</param>
		/// <param name="name">インデックスバッファの名前（デバック用）</param>
		void Create(ID3D12Device* device, uint32_t indexCount, const std::string& name = "IndexBuffer");
		/// <summary>
		/// インデックスを書き込む
		/// </summary>
		/// <param name="indices">インデックス</param>
		void WriteData(const Index* indices);

		uint32_t GetBufferSize() const { return  kStrideSize * indexCount_; }
		uint32_t GetIndexCount() const { return indexCount_; }
		D3D12_INDEX_BUFFER_VIEW GetView() const { return { GetGPUAddress(), GetBufferSize(), DXGI_FORMAT_R16_UINT }; }

	private:
		uint32_t indexCount_{ 0 };
	};
#pragma endregion インデックスバッファ
#pragma region 定数バッファ
	class ConstantBuffer : public GPUResource {
	public:
		ConstantBuffer() = default;
		DELETE_COPY_MOVE(ConstantBuffer);
		~ConstantBuffer();
		void Create(ID3D12Device* device, uint32_t dataSize, const std::string& name = "ConstantBuffer");
		void WriteData(void* data);

		void* GetMapPtr() const { return mapPtr_; }
		uint32_t GetBufferSize() const { return bufferSize_; }
		uint32_t GetDataSize() const { return dataSize_; }

	private:
		void* mapPtr_{ nullptr };
		uint32_t bufferSize_{ 0 };
		uint32_t dataSize_{ 0 };
	};
#pragma endregion 定数バッファ
#pragma region 構造化バッファ
	class StructuredBuffer : public GPUResource {
	public:
		StructuredBuffer() = default;
		DELETE_COPY_MOVE(StructuredBuffer);
		~StructuredBuffer();
		void Create(ID3D12Device* device, uint32_t elementSize, uint32_t elementCount, const std::string& name = "StructuredBuffer");
		void WriteData(void* data);

		void* GetMapPtr() const { return mapPtr_; }
		uint32_t GetBufferSize() const { return elementCount_ * elementSize_; }
		uint32_t GetElementCount() const { return elementCount_; }
		uint32_t GetElementSize() const { return elementSize_; }

	private:
		void* mapPtr_{ nullptr };
		uint32_t elementCount_{ 0 };
		uint32_t elementSize_{ 0 };
	};
#pragma endregion 構造化バッファ
};

#define ASSERT_HRESULT(hr) { if(FAILED(hr)) assert(false); }

namespace DX12Wrapper {
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
}