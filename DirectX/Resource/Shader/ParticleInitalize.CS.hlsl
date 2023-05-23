struct Particle {
	float32_t3 position;
	float32_t3 velocity;
	float32_t3 acceleration;
};

RWStructuredBuffer<Particle> g_particles : register(u0);

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID) {
	g_particles[DTid.x].acceleration = float32_t3(0.0f, 0.0f, 0.0f);
	g_particles[DTid.x].velocity = float32_t3(0.0f, 0.0f, 0.0f);
	g_particles[DTid.x].position = float32_t3(DTid.x, DTid.x, 20.0f);
}