#define HLSL
#include "ParticleGraphics_HLSLCompat.h"

ConstantBuffer<ParticleShader::Scene> sceneCB : register(b0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
};

struct VertexShaderOutput {
	float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(input.position, sceneCB.viewMatrix);
	return output;
}