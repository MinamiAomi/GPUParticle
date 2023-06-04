#pragma once
namespace DirectXHelper {
	using namespace Microsoft::WRL;

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

		operator const D3D12_GRAPHICS_PIPELINE_STATE_DESC& ();

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
		operator const D3D12_COMPUTE_PIPELINE_STATE_DESC& ();

		void SetRootSignature(ID3D12RootSignature* rootSignature);
		void SeComputeShader(const void* shaderBytecode, SIZE_T bytecodeLength);
		
	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc_{};

		friend class PipelineState;
	};

	class PipelineState {
	public:
		operator ID3D12PipelineState* () const { return pipelineState_.Get(); }

		bool Initalize(ID3D12Device* device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
		bool Initalize(ID3D12Device* device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc);
	
		bool IsEnabled() const { return pipelineState_; }
		ID3D12PipelineState* Get() const { return pipelineState_.Get(); }
		ComPtr<ID3D12PipelineState> GetComPtr() const { return pipelineState_; }
		ID3D12PipelineState* const* GetAddressOf() const { return pipelineState_.GetAddressOf(); }

		void SetName(const std::wstring& name);

	private:
		ComPtr<ID3D12PipelineState> pipelineState_;
	};

}
