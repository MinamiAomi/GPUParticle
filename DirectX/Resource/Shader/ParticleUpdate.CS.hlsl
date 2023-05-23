struct Particle {
	float32_t3 position;
	float32_t3 velocity;
	float32_t3 acceleration;
};

struct Information {
	float32_t3 target;
};

RWStructuredBuffer<Particle> g_particles : register(u0);
ConstantBuffer<Information> g_info : register(b0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	float32_t3 dir = normalize(g_info.target - g_particles[DTid.x].position);
	
	g_particles[DTid.x].acceleration = dir * 0.001f;
	g_particles[DTid.x].velocity += g_particles[DTid.x].acceleration;
	g_particles[DTid.x].position += g_particles[DTid.x].velocity;
}