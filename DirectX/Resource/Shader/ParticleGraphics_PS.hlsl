struct PixelShaderInput {
	float32_t4 position : SV_POSITION0;
	float32_t2 uv : TEXCOORD0;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(PixelShaderInput input) {
	float alpha = 1.0f - length(input.uv);
	PixelShaderOutput output;
	output.color = float32_t4(0.01f, 0.01f, 0.01f, alpha);
	return output;
}