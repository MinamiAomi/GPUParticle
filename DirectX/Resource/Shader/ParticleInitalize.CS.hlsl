#define HLSL
#include "ParticleCompute_HLSLCompat.h"

RWStructuredBuffer<ParticleShader::Particle> particlesRWSB : register(u0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	particlesRWSB[DTid.x].acceleration = float32_t3(0.0f, 0.0f, 0.0f);
	particlesRWSB[DTid.x].velocity = float32_t3(0.0f, 0.0f, 0.0f);
	particlesRWSB[DTid.x].position = float32_t3(DTid.x, DTid.x, 200.1f);
}