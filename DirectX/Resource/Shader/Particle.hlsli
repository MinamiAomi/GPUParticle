struct TransformCB {
	float32_t4x4 viewMatrix;
	float32_t4x4 projectionMatrix;
};

ConstantBuffer<TransformCB> g_transformCB : register(b0);