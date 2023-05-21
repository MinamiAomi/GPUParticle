#include "Particle.hlsli"

struct GeometryShaderInput {
	float32_t4 position : POSITION;
};

struct GeometryShaderOutput {
	float32_t4 position : SV_POSITION;
};

[maxvertexcount(3)]
void main(
	point GeometryShaderInput input[1],
	inout TriangleStream<GeometryShaderOutput> output
)
{
	GeometryShaderOutput elements[3];

	elements[0].position = input[0].position;
	elements[0].position.y += 1.0f;

	elements[1].position = input[0].position;
	elements[1].position.x += 1.0f;
	elements[1].position.y -= 1.0f;
	
	elements[2].position = input[0].position;
	elements[2].position.x -= 1.0f;
	elements[2].position.y -= 1.0f;

	for (uint32_t i = 0; i < 3; ++i) {
		elements[i].position = mul(elements[i].position, g_transformCB.projectionMatrix);
		output.Append(elements[i]);
	}
}