#define HLSL
#include "ParticleCompute_HLSLCompat.h"

RWStructuredBuffer<ParticleShader::Particle> particlesRWSB : register(u0);
ConstantBuffer<ParticleShader::Target> targetCB : register(b0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	float32_t3 dir = normalize(targetCB.position - particlesRWSB[DTid.x].position);
	
	particlesRWSB[DTid.x].acceleration = dir * 0.001f;
	particlesRWSB[DTid.x].velocity += particlesRWSB[DTid.x].acceleration;
	particlesRWSB[DTid.x].position += particlesRWSB[DTid.x].velocity;
}