#pragma once
#include "DirectXDevice.h"
#include "Resource/Shader/ParticleCompute_HLSLCompat.h"
#include "Resource/Shader/ParticleGraphics_HLSLCompat.h"


class GPUParticle {
public:
	static const uint32_t kParticleCount = 10000;

	void Initalize(DirectXDevice& directXDevice);
	void Update(ID3D12GraphicsCommandList* cmdList);
	void Draw(ID3D12GraphicsCommandList* cmdList);

public:
	DirectXHelper::RootSignature rootSignature_;

	DirectXHelper::PipelineState initPipelineState_;
	DirectXHelper::PipelineState updatePipelineState_;


	DirectXHelper::GPUResource particlesRWSB_;
	DirectXHelper::ConstantBuffer targetCB_;

	ParticleShader::Target target_;

};

