#include "DirectXHelper.h"
#include "StringUtils.h"

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
		D3D12_ROOT_PARAMETER rootParameter{
			.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
			.DescriptorTable{},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
		};
		rootParameters_.emplace_back(rootParameter);
		ranges_.emplace_back();
	}

	void RootSignatureDesc::AddDescriptor(DescriptorType type, uint32_t shaderRegister, uint32_t registerSpace) {
		D3D12_ROOT_PARAMETER rootParameter{
			.ParameterType = static_cast<D3D12_ROOT_PARAMETER_TYPE>(type),
			.Descriptor{
				.ShaderRegister = shaderRegister,
				.RegisterSpace = registerSpace
			},
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
		};
		rootParameters_.emplace_back(rootParameter);
	}

	void RootSignatureDesc::AddDescriptorRange(RangeType rangeType, uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, uint32_t offset) {
		if (ranges_.empty()) {
			AddDescriptorTable();
		}
		D3D12_DESCRIPTOR_RANGE descriptorRange{
			.RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(rangeType),
			.NumDescriptors = numDescriptors,
			.BaseShaderRegister = baseShaderRegister,
			.RegisterSpace = registerSpace,
			.OffsetInDescriptorsFromTableStart = offset
		};
		ranges_.back().emplace_back(descriptorRange);
	}

	void RootSignatureDesc::AddStaticSampler(uint32_t shaderRegister, uint32_t ragisterSpace, D3D12_FILTER filter, AddressMode addressU, AddressMode addressV, AddressMode addressW) {
		D3D12_STATIC_SAMPLER_DESC staticSamplerDesc{
			.Filter = filter,
			.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressU),
			.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressV),
			.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressW),
			.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
			.MinLOD = 0.0f,
			.MaxLOD = D3D12_FLOAT32_MAX,
			.ShaderRegister = shaderRegister,
			.RegisterSpace = ragisterSpace,
			.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
		};
		staticSamplerDescs_.emplace_back(staticSamplerDesc);
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

		CHECK_HRESULT(D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			blob.GetAddressOf(),
			errorBlob.GetAddressOf()));

		CHECK_HRESULT(device->CreateRootSignature(
			0,
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf())));
		rootSignature_->SetName(String::Convert(name).c_str());
	}
#pragma endregion ルートシグネチャ
#pragma region 頂点バッファ
	void VertexBuffer::Create(ID3D12Device* device, size_t vertexCount, size_t strideSize, const std::string& name) {
		assert(device);

		vertexCount_ = static_cast<uint32_t>(vertexCount);
		bufferView_.StrideInBytes = static_cast<uint32_t>(strideSize);
		bufferView_.SizeInBytes = bufferView_.StrideInBytes * vertexCount_;

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<uint64_t>(bufferView_.SizeInBytes));

		CHECK_HRESULT(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(buffer_.ReleaseAndGetAddressOf())));
		buffer_->SetName(String::Convert(name).c_str());

		bufferView_.BufferLocation = buffer_->GetGPUVirtualAddress();
	}

	void VertexBuffer::WriteData(const void* vertices) {
		assert(buffer_);
		void* map = nullptr;
		CHECK_HRESULT(buffer_->Map(0, nullptr, &map));
		memcpy(map, vertices, bufferView_.SizeInBytes);
		buffer_->Unmap(0, nullptr);
		map = nullptr;
	}
#pragma endregion 頂点バッファ
#pragma region インデックスバッファ
	void IndexBuffer::Create(ID3D12Device* device, size_t indexCount, const std::string& name) {
		assert(device);

		indexCount_ = static_cast<uint32_t>(indexCount);
		bufferView_.Format = DXGI_FORMAT_R16_UINT;
		bufferView_.SizeInBytes = static_cast<uint32_t>(sizeof(uint16_t)) * indexCount_;

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<uint64_t>(bufferView_.SizeInBytes));

		CHECK_HRESULT(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(buffer_.ReleaseAndGetAddressOf())));
		buffer_->SetName(String::Convert(name).c_str());

		bufferView_.BufferLocation = buffer_->GetGPUVirtualAddress();
	}

	void IndexBuffer::WriteData(const uint16_t* indices) {
		assert(buffer_);
		void* map = nullptr;
		CHECK_HRESULT(buffer_->Map(0, nullptr, &map));
		memcpy(map, indices, bufferView_.SizeInBytes);
		buffer_->Unmap(0, nullptr);
		map = nullptr;
	}
#pragma endregion インデックスバッファ
#pragma region 定数バッファ
	ConstantBuffer::~ConstantBuffer() {
		if (mapPtr_) {
			buffer_->Unmap(0, nullptr);
			mapPtr_ = nullptr;
		}
	}
	void ConstantBuffer::Create(ID3D12Device* device, size_t size, const std::string& name) {
		assert(device);

		if (mapPtr_) {
			buffer_->Unmap(0, nullptr);
			mapPtr_ = nullptr;
		}
		
		dataSize_ = static_cast<uint32_t>(size);
		alignedSize_ = static_cast<uint32_t>(Align(size, 256));

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedSize_);
		
		CHECK_HRESULT(device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(buffer_.ReleaseAndGetAddressOf())));
		buffer_->SetName(String::Convert(name).c_str());
		
		CHECK_HRESULT(buffer_->Map(0, nullptr, &mapPtr_));
		memset(mapPtr_, 0, alignedSize_);
	}
	void ConstantBuffer::WriteData(void* data) {
		memcpy(mapPtr_, data, dataSize_);
	}
#pragma endregion 定数バッファ
};