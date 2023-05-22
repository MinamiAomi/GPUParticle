struct Particle {
	float32_t3 position;
	float32_t3 velocity;
	float32_t3 acceleration;
};

ConstantBuffer<float32_t3> g_target : register(c0);
RWStructuredBuffer<Particle> g_particles : register(u0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	float32_t3 dir = normalize(g_target - g_particle[DTid.x].position);
	
	g_particle[DTid.x].acceleration = dir * 0.1f;
	g_particle[DTid.x].velocity += g_particle[DTid.x].acceleration;
	g_particle[DTid.x].position += g_particle[DTid.x].velocity;
}