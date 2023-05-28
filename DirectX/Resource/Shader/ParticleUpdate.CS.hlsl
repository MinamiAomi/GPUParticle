#define HLSL
#include "ParticleCompute_HLSLCompat.h"

RWStructuredBuffer<ParticleShader::Particle> particlesRWSB : register(u0);
ConstantBuffer<ParticleShader::Target> targetCB : register(b0);

[numthreads(16, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	float32_t3 direction = targetCB.position - particlesRWSB[DTid.x].position.xyz;
	float32_t distance = length(direction);
	direction = direction / distance;
	if (distance <= 1.0f) {
	//	direction *= -1.0f;
		//particlesRWSB[DTid.x].velocity *= -1.0f;
	}


	particlesRWSB[DTid.x].acceleration = direction * 0.001f;
	particlesRWSB[DTid.x].velocity += particlesRWSB[DTid.x].acceleration;
	particlesRWSB[DTid.x].position.xyz += particlesRWSB[DTid.x].velocity;
}