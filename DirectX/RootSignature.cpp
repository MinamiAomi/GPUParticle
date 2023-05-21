#include "RootSignature.h"
#include <cassert>
#include "StringUtils.h"

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

void RootSignature::Create(ComPtr<ID3D12Device5> device, RootSignatureDesc& rootSignatureDesc, const std::string& name) {
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
	assert(SUCCEEDED(hr));

	hr = device->CreateRootSignature(
		0, 
		blob->GetBufferPointer(), 
		blob->GetBufferSize(), 
		IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));
	rootSignature_->SetName(ConvertString(name).c_str());
}
