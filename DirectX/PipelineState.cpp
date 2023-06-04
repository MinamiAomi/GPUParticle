#include "stdafx.h"
#include "PipelineState.h"
#include "Logger.h"

namespace DirectXHelper {

	void GraphicsPipelineStateDesc::SetRootSignature(ID3D12RootSignature* rootSignature) {
		desc_.pRootSignature = rootSignature;
	}

	void GraphicsPipelineStateDesc::SetVertexShader(const void* shaderBytecode, SIZE_T bytecodeLength) {
		desc_.VS.pShaderBytecode = shaderBytecode;
		desc_.VS.BytecodeLength = bytecodeLength;
	}

	void GraphicsPipelineStateDesc::SetPixelShader(const void* shaderBytecode, SIZE_T bytecodeLength) {
		desc_.PS.pShaderBytecode = shaderBytecode;
		desc_.PS.BytecodeLength = bytecodeLength;
	}

	void GraphicsPipelineStateDesc::SetDomainShader(const void* shaderBytecode, SIZE_T bytecodeLength) {
		desc_.DS.pShaderBytecode = shaderBytecode;
		desc_.DS.BytecodeLength = bytecodeLength;
	}

	void GraphicsPipelineStateDesc::SetHullShader(const void* shaderBytecode, SIZE_T bytecodeLength) {
		desc_.HS.pShaderBytecode = shaderBytecode;
		desc_.HS.BytecodeLength = bytecodeLength;
	}

	void GraphicsPipelineStateDesc::SetGeometryShader(const void* shaderBytecode, SIZE_T bytecodeLength) {
		desc_.GS.pShaderBytecode = shaderBytecode;
		desc_.GS.BytecodeLength = bytecodeLength;
	}

	void GraphicsPipelineStateDesc::SetRasterizerState(FillMode fillMode, CullMode cullMode) {
		auto& rs = desc_.RasterizerState;
		rs.FillMode = static_cast<D3D12_FILL_MODE>(fillMode);
		rs.CullMode = static_cast<D3D12_CULL_MODE>(cullMode);
	}

	void GraphicsPipelineStateDesc::AddInputElementVertex(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot) {
		auto& inputElement = inputElements_.emplace_back();
		// 一時バッファに保存
		semanticNames.emplace_back(semanticName);
		inputElement.SemanticName = semanticNames.back().c_str();
		inputElement.SemanticIndex = semanticIndex;
		inputElement.Format = format;
		inputElement.InputSlot = inputSlot;
		inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate = 0;
	}

	void GraphicsPipelineStateDesc::AddInputElementInstance(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot, uint32_t instanceDataStepRate) {
		auto& inputElement = inputElements_.emplace_back();
		semanticNames.emplace_back(semanticName);
		inputElement.SemanticName = semanticNames.back().c_str();
		inputElement.SemanticIndex = semanticIndex;
		inputElement.Format = format;
		inputElement.InputSlot = inputSlot;
		inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		inputElement.InstanceDataStepRate = instanceDataStepRate;
	}

	void GraphicsPipelineStateDesc::SetPrimitiveTopologyType(PrimitiveTopology primitiveTopologyType) {
		desc_.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(primitiveTopologyType);
	}

	void GraphicsPipelineStateDesc::AddRenderTargetState(BlendMode blendMode, DXGI_FORMAT rtvFormat) {
		assert(renderTargetCount_ < 8);
		auto& rt = desc_.BlendState.RenderTarget[renderTargetCount_];
		desc_.RTVFormats[renderTargetCount_] = rtvFormat;
		++renderTargetCount_;

		rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;			// 加算
		rt.SrcBlendAlpha = D3D12_BLEND_ONE;			// ソースの値を 100% 使う
		rt.DestBlendAlpha = D3D12_BLEND_ZERO;			// デストの値を   0% 使う
		rt.BlendEnable = true;						// ブレンドを有効にする

		switch (blendMode)
		{
		case DirectXHelper::GraphicsPipelineStateDesc::BlendMode::None:
			rt.BlendEnable = false;
			return;
		case DirectXHelper::GraphicsPipelineStateDesc::BlendMode::Normal:
		default:
			rt.BlendOp = D3D12_BLEND_OP_ADD;
			rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	// 1.0f-ソースのアルファ値
			return;
		case DirectXHelper::GraphicsPipelineStateDesc::BlendMode::Add:
			rt.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
			rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;				// ソースの値を 100% 使う
			rt.DestBlend = D3D12_BLEND_ONE;				// デストの値を 100% 使う
			return;
		case DirectXHelper::GraphicsPipelineStateDesc::BlendMode::Subtract:
			rt.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	// デストからソースを減算
			rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;				// ソースの値を 100% 使う
			rt.DestBlend = D3D12_BLEND_ONE;				// デストの値を 100% 使う
			return;
		case DirectXHelper::GraphicsPipelineStateDesc::BlendMode::Multiply:
			rt.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
			rt.SrcBlend = D3D12_BLEND_ZERO;				// 使わない
			rt.DestBlend = D3D12_BLEND_SRC_COLOR;		// デストの値 × ソースの値
			return;
		case DirectXHelper::GraphicsPipelineStateDesc::BlendMode::Inverse:
			rt.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
			rt.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	// 1.0f-デストカラーの値
			rt.DestBlend = D3D12_BLEND_ZERO;				// 使わない
			return;
		}
	}

	void GraphicsPipelineStateDesc::SetDepthState(DepthWriteMask depthWriteMask, ComparisonFunc comparisonFunc, DXGI_FORMAT dsvFormat) {
		auto& dss = desc_.DepthStencilState;
		dss.DepthEnable = true;
		dss.DepthWriteMask = static_cast<D3D12_DEPTH_WRITE_MASK>(depthWriteMask);
		dss.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(comparisonFunc);
		desc_.DSVFormat = dsvFormat;
	}

	void GraphicsPipelineStateDesc::SetSampleState(uint32_t count, uint32_t quality) {
		desc_.SampleDesc.Count = count;
		desc_.SampleDesc.Quality = quality;
	}

	GraphicsPipelineStateDesc::operator const D3D12_GRAPHICS_PIPELINE_STATE_DESC& () {
		desc_.NumRenderTargets = renderTargetCount_;
		desc_.InputLayout.pInputElementDescs = inputElements_.data();
		desc_.InputLayout.NumElements = static_cast<uint32_t>(inputElements_.size());
		desc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		return desc_;
	}

	void ComputePipelineStateDesc::SetRootSignature(ID3D12RootSignature* rootSignature) {
		desc_.pRootSignature = rootSignature;
	}

	void ComputePipelineStateDesc::SeComputeShader(const void* shaderBytecode, SIZE_T bytecodeLength) {
		desc_.CS.pShaderBytecode = shaderBytecode;
		desc_.CS.BytecodeLength = bytecodeLength;
	}

	ComputePipelineStateDesc::operator const D3D12_COMPUTE_PIPELINE_STATE_DESC& () {
		return desc_;
	}


	bool PipelineState::Initalize(ID3D12Device* device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
		assert(device);

		if (FAILED(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())))) {
			Logger::Error("CreateGraphicsPipelineState()");
			assert(false);
			return false;
		}
		return true;
	}

	bool PipelineState::Initalize(ID3D12Device* device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc) {
		assert(device);

		if (FAILED(device->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())))) {
			Logger::Error("CreateComputePipelineState()");
			assert(false);
			return false;
		}
		return true;
	}

	void PipelineState::SetName(const std::wstring& name) {
		assert(pipelineState_);
		pipelineState_->SetName(name.c_str());
	}


}
