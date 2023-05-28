#define HLSL
#include "ParticleCompute_HLSLCompat.h"

RWStructuredBuffer<ParticleShader::Particle> particlesRWSB : register(u0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	particlesRWSB[DTid.x].acceleration = float32_t3(0.0f, 0.0f, 0.0f);
	float32_t ring = 65536.0f / 10.0f;
	float32_t thetaEvery = 2.0f * 3.141592653589793f / ring;
	float32_t theta = float32_t(DTid.x) * thetaEvery;

	uint32_t a = DTid.x / (65536 / 10);

	float32_t dis = 0.2f * float32_t(a);
	float32_t s = sin(theta);
	float32_t c = cos(theta);
	float32_t initSpeed = 0.01f;
	particlesRWSB[DTid.x].velocity = float32_t3(c * initSpeed, s * initSpeed, 0.0f);
	particlesRWSB[DTid.x].position = float32_t4(c * dis, s * dis, 0.0f, 1.0f);
}