#include "Particle.hlsli"

struct GeometryShaderInput {
	float32_t4 position : POSITION0;
};

struct GeometryShaderOutput {
	float32_t4 position : SV_POSITION0;
	float32_t2 uv : TEXCOORD0;
};

[maxvertexcount(4)]
void main(
	point GeometryShaderInput input[1],
	inout TriangleStream<GeometryShaderOutput> output
)
{
	float32_t2 pos[4] = {
		float32_t2( -1.0f,  1.0f ),
		float32_t2(  1.0f,  1.0f ),
		float32_t2( -1.0f, -1.0f ),
		float32_t2(  1.0f, -1.0f ),
	};
	for (uint32_t i = 0; i < 4; ++i) {
		GeometryShaderOutput e;
		float32_t2 newPos = input[0].position.xy + pos[i];
		e.position = mul(float32_t4(newPos, input[0].position.zw), g_transformCB.projectionMatrix);
		e.uv = pos[i].xy;
		output.Append(e);
	}
	output.RestartStrip();
}