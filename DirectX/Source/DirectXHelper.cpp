#include "DirectXHelper.h"
#include "StringUtils.h"
#include "Debug.h"

namespace DirectXHelper {

	ComPtr<ID3D12Resource> CreateBuffer(ID3D12Device* device, uint64_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState, D3D12_HEAP_TYPE heapType) {
		auto heapProps = CD3DX12_HEAP_PROPERTIES(heapType);
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size, flags);

		ComPtr<ID3D12Resource> buffer;
		CHECK_HRESULT(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			initState,
			nullptr, IID_PPV_ARGS(buffer.GetAddressOf())));

		return buffer;
	}

	ComPtr<ID3D12Resource> CreateBufferUAV(ID3D12Device* device, uint64_t size, D3D12_RESOURCE_STATES initState) {
		return CreateBuffer(device, size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, initState, D3D12_HEAP_TYPE_DEFAULT);
	}

	void WriteToBuffer(ID3D12Resource* buffer, size_t byteSize, void* source) {
		void* destination{ nullptr };
		CHECK_HRESULT(buffer->Map(0, nullptr, &destination));
		memset(destination, 0, buffer->GetDesc().Width);
		memcpy(destination, source, byteSize);
		buffer->Unmap(0, nullptr);
	}

	uint64_t Align(uint64_t value, uint64_t alignment) {
		return (value + alignment - 1) & ~(alignment - 1);
	};
	uint32_t Align(uint32_t value, uint32_t alignment) {
		return (value + alignment - 1) & ~(alignment - 1);
	};

#pragma region ディスクリプタヒープ
	void DescriptorHeap::Initalize(ID3D12Device* device, uint32_t capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, bool isShaderVisible, const std::string& name) {
		assert(device);
		assert(capacity);
		assert(capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);

		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
			isShaderVisible = false;
		}

		D3D12_DESCRIPTOR_HEAP_DESC desc{
			.Type = type,
			.NumDescriptors = capacity,
			.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};

		CHECK_HRESULT(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap_.ReleaseAndGetAddressOf())));
		heap_->SetName(String::Convert(name).c_str());

		cpuStartHandle_ = heap_->GetCPUDescriptorHandleForHeapStart();
		gpuStartHandle_ = isShaderVisible ? heap_->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
		descriptorSize_ = device->GetDescriptorHandleIncrementSize(type);
		capacity_ = capacity;
		allocateIndex_ = 0;
		type_ = type;
	}

	Descriptor DescriptorHeap::Allocate() {
		assert(heap_);
		assert(allocateIndex_ < capacity_);

		uint32_t offset = allocateIndex_ * descriptorSize_;
		Descriptor result{};
		result.cpu.ptr = cpuStartHandle_.ptr + offset;
		if (IsShaderVisible()) {
			result.gpu.ptr = gpuStartHandle_.ptr + offset;
		}
#ifdef _DEBUG
		result.offset = offset;
		result.container = this;
#endif // _DEBUG
		++allocateIndex_;
		return result;
	}

	void DescriptorHeap::Deallocate(Descriptor& descriptorHandle) {
		assert(descriptorHandle.IsValid());
		assert(heap_ && allocateIndex_);
		assert(descriptorHandle.container == this);
		assert(descriptorHandle.cpu.ptr >= cpuStartHandle_.ptr);
		assert((descriptorHandle.cpu.ptr - cpuStartHandle_.ptr) % descriptorSize_ == 0);
		assert(descriptorHandle.offset < capacity_);

		descriptorHandle = {};
	}
#pragma endregion ディスクリプタヒープ
#pragma region ルートシグネチャ
	void RootSignatureDesc::AddDescriptorTable() {
		ranges_.emplace_back();
		auto& rootParameter = rootParameters_.emplace_back();
		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter.DescriptorTable = {};
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}

	void RootSignatureDesc::AddDescriptor(DescriptorType type, uint32_t shaderRegister, uint32_t registerSpace) {
		auto& rootParameter = rootParameters_.emplace_back();
		rootParameter.ParameterType = static_cast<D3D12_ROOT_PARAMETER_TYPE>(type);
		rootParameter.Descriptor.ShaderRegister = shaderRegister;
		rootParameter.Descriptor.RegisterSpace = registerSpace;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}

	void RootSignatureDesc::AddDescriptorRange(RangeType rangeType, uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, uint32_t offset) {
		assert(numDescriptors > 0);
		if (ranges_.empty()) {
			AddDescriptorTable();
		}
		auto& descriptorRange = ranges_.back().emplace_back();
		descriptorRange.RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(rangeType);
		descriptorRange.NumDescriptors = numDescriptors;
		descriptorRange.BaseShaderRegister = baseShaderRegister;
		descriptorRange.RegisterSpace = registerSpace;
		descriptorRange.OffsetInDescriptorsFromTableStart = offset;
	}

	void RootSignatureDesc::AddStaticSampler(uint32_t shaderRegister, uint32_t ragisterSpace, D3D12_FILTER filter, AddressMode addressU, AddressMode addressV, AddressMode addressW) {
		auto& staticSamplerDesc = staticSamplerDescs_.emplace_back();
		staticSamplerDesc.Filter = filter;
		staticSamplerDesc.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressU);
		staticSamplerDesc.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressV);		
		staticSamplerDesc.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressW);
		staticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplerDesc.MinLOD = 0.0f;
		staticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplerDesc.ShaderRegister = shaderRegister;
		staticSamplerDesc.RegisterSpace = ragisterSpace;
		staticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	}

	void RootSignatureDesc::AddFlags(D3D12_ROOT_SIGNATURE_FLAGS flags) {
		flags_ |= flags;
	}

	void RootSignatureDesc::Clear() {
		ranges_.clear();
		rootParameters_.clear();
		staticSamplerDescs_.clear();
		flags_ = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	}

	void RootSignature::Create(ID3D12Device* device, RootSignatureDesc& rootSignatureDesc, const std::string& name) {
		assert(device);
		auto ranges = rootSignatureDesc.ranges_.begin();
		for (auto& itr : rootSignatureDesc.rootParameters_) {
			if (itr.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
				itr.DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(ranges->size());
				itr.DescriptorTable.pDescriptorRanges = ranges->data();
				++ranges;
			}
		}

		D3D12_ROOT_SIGNATURE_DESC desc{};
		if (!rootSignatureDesc.rootParameters_.empty()) {
			desc.NumParameters = static_cast<uint32_t>(rootSignatureDesc.rootParameters_.size());
			desc.pParameters = rootSignatureDesc.rootParameters_.data();
		}
		if (!rootSignatureDesc.staticSamplerDescs_.empty()) {
			desc.NumStaticSamplers = static_cast<uint32_t>(rootSignatureDesc.staticSamplerDescs_.size());
			desc.pStaticSamplers = rootSignatureDesc.staticSamplerDescs_.data();
		}
		desc.Flags = rootSignatureDesc.flags_;

		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			blob.GetAddressOf(),
			errorBlob.GetAddressOf());
		if (FAILED(hr)) {
			Debug::Log(static_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}

		CHECK_HRESULT(device->CreateRootSignature(
			0,
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf())));
		rootSignature_->SetName(String::Convert(name).c_str());
	}
#pragma endregion ルートシグネチャ
#pragma region パイプライン
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

	void GraphicsPipelineStateDesc::SetPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType) {
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

	void ComputePipelineStateDesc::SetRootSignature(ID3D12RootSignature* rootSignature)	{
		desc_.pRootSignature = rootSignature;
	}
	
	void ComputePipelineStateDesc::SeComputeShader(const void* shaderBytecode, SIZE_T bytecodeLength) {
		desc_.CS.pShaderBytecode = shaderBytecode;
		desc_.CS.BytecodeLength = bytecodeLength;
	}

	void PipelineState::Create(ID3D12Device* device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::string& name) {
		assert(device);

		CHECK_HRESULT(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
		pipelineState_->SetName(String::Convert(name).c_str());
	}

	void PipelineState::Create(ID3D12Device* device, GraphicsPipelineStateDesc& desc, const std::string& name) {
		assert(device);

		desc.desc_.NumRenderTargets = desc.renderTargetCount_;
		desc.desc_.InputLayout.pInputElementDescs = desc.inputElements_.data();
		desc.desc_.InputLayout.NumElements = static_cast<uint32_t>(desc.inputElements_.size());
		desc.desc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		Create(device, desc.desc_, name);
	}

	void PipelineState::Create(ID3D12Device* device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc, const std::string& name) {
		assert(device);

		CHECK_HRESULT(device->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
		pipelineState_->SetName(String::Convert(name).c_str());
	}

	void PipelineState::Create(ID3D12Device* device, ComputePipelineStateDesc& desc, const std::string& name) {
		assert(device);
		
		Create(device, desc.desc_, name);
	}

#pragma endregion パイプライン
#pragma region GPUリソース
	void GPUResource::Create(ID3D12Device* device, const D3D12_HEAP_PROPERTIES& heapProperties, const D3D12_RESOURCE_DESC& resourceDesc, D3D12_RESOURCE_STATES initialResourceState, const std::string& name) {
		assert(device);
		CHECK_HRESULT(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			initialResourceState,
			nullptr,
			IID_PPV_ARGS(gpuResource_.ReleaseAndGetAddressOf())));
		gpuResource_->SetName(String::Convert(name).c_str());

	}
#pragma endregion GPUリソース
#pragma region 頂点バッファ
	void VertexBuffer::Create(ID3D12Device* device, uint32_t vertexCount, uint32_t strideSize, const std::string& name) {
		assert(device);

		vertexCount_ = vertexCount;
		strideSize_ = strideSize;
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<uint64_t>(GetBufferSize()));
		GPUResource::Create(device, heapProperties, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, name);
	}

	void VertexBuffer::WriteData(const void* vertices) {
		assert(IsEnabled());
		void* map = nullptr;
		CHECK_HRESULT(gpuResource_->Map(0, nullptr, &map));
		memcpy(map, vertices, GetBufferSize());
		gpuResource_->Unmap(0, nullptr);
		map = nullptr;
	}
#pragma endregion 頂点バッファ
#pragma region インデックスバッファ
	void IndexBuffer::Create(ID3D12Device* device, uint32_t indexCount, const std::string& name) {
		assert(device);

		indexCount_ = indexCount;
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<uint64_t>(GetBufferSize()));
		GPUResource::Create(device, heapProperties, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, name);
	}

	void IndexBuffer::WriteData(const Index* indices) {
		assert(IsEnabled());
		void* map = nullptr;
		CHECK_HRESULT(gpuResource_->Map(0, nullptr, &map));
		memcpy(map, indices, GetBufferSize());
		gpuResource_->Unmap(0, nullptr);
		map = nullptr;
	}
#pragma endregion インデックスバッファ
#pragma region 定数バッファ
	ConstantBuffer::~ConstantBuffer() {
		if (mapPtr_) {
			gpuResource_->Unmap(0, nullptr);
			mapPtr_ = nullptr;
		}
	}
	void ConstantBuffer::Create(ID3D12Device* device, uint32_t dataSize, const std::string& name) {
		assert(device);

		if (mapPtr_) {
			gpuResource_->Unmap(0, nullptr);
			mapPtr_ = nullptr;
		}
		
		dataSize_ = dataSize;
		bufferSize_ = Align(dataSize_, 256);

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize_);
		GPUResource::Create(device, heapProperties, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, name);
		
		CHECK_HRESULT(gpuResource_->Map(0, nullptr, &mapPtr_));
		memset(mapPtr_, 0, bufferSize_);
	}
	void ConstantBuffer::WriteData(void* data) {
		assert(IsEnabled());
		assert(mapPtr_);
		memcpy(mapPtr_, data, dataSize_);
	}
#pragma endregion 定数バッファ
#pragma region 構造化バッファ

	StructuredBuffer::~StructuredBuffer() {
		if (mapPtr_) {
			gpuResource_->Unmap(0, nullptr);
			mapPtr_ = nullptr;
		}
	}

	void StructuredBuffer::Create(ID3D12Device* device, uint32_t elementSize, uint32_t elementCount, const std::string& name) {
		assert(device);

		if (mapPtr_) {
			gpuResource_->Unmap(0, nullptr);
			mapPtr_ = nullptr;
		}

		elementSize_ = elementSize;
		elementCount_ = elementCount;

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<uint64_t>(GetBufferSize()));
		GPUResource::Create(device, heapProperties, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, name);

		CHECK_HRESULT(gpuResource_->Map(0, nullptr, &mapPtr_));
		memset(mapPtr_, 0, static_cast<uint64_t>(GetBufferSize()));
	}

	void StructuredBuffer::WriteData(void* data) {
		memcpy(mapPtr_, data, static_cast<uint64_t>(GetBufferSize()));
	}

#pragma endregion 構造化バッファ

};