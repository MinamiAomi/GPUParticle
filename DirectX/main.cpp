#include "Window.h"
#include "DirectXDevice.h"
#include "ShaderCompiler.h"
#include "MathUtils.h"

using namespace Math;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Window window;
	window.Create();

	DirectXDevice directXDevice;
	directXDevice.Initalize(window.GetHWND(), window.GetClientWidth(), window.GetClientHeight());

	ShaderCompiler::Initalize();

	window.Show();

	DirectXHelper::RootSignature rs;
	DirectXHelper::PipelineState pso;
	{
		using DescriptorType = DirectXHelper::RootSignatureDesc::DescriptorType;
		using RangeType = DirectXHelper::RootSignatureDesc::RangeType;
		using FillMode = DirectXHelper::GraphicsPipelineStateDesc::FillMode;
		using CullMode = DirectXHelper::GraphicsPipelineStateDesc::CullMode;
		using PrimitiveTopologyType = DirectXHelper::GraphicsPipelineStateDesc::PrimitiveTopologyType;
		using BlendMode = DirectXHelper::GraphicsPipelineStateDesc::BlendMode;
		using DepthWriteMask = DirectXHelper::GraphicsPipelineStateDesc::DepthWriteMask;
		using ComparisonFunc = DirectXHelper::GraphicsPipelineStateDesc::ComparisonFunc;


		DirectXHelper::RootSignatureDesc rsDesc;
		rsDesc.AddDescriptor(DescriptorType::CBV, 0);
		rsDesc.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rs.Create(directXDevice.GetDevice(), rsDesc);

		auto vs = ShaderCompiler::Compile(L"Resource/Shader/Particle.VS.hlsl", L"main", L"vs_6_0");
		auto ps = ShaderCompiler::Compile(L"Resource/Shader/Particle.PS.hlsl", L"main", L"ps_6_0");
		auto gs = ShaderCompiler::Compile(L"Resource/Shader/Particle.GS.hlsl", L"main", L"gs_6_0");


		DirectXHelper::GraphicsPipelineStateDesc psDesc;
		psDesc.SetRootSignature(rs.Get());
		psDesc.SetVertexShader(vs->GetBufferPointer(), vs->GetBufferSize());
		psDesc.SetPixelShader(ps->GetBufferPointer(), ps->GetBufferSize());
		psDesc.SetGeometryShader(gs->GetBufferPointer(), gs->GetBufferSize());
		psDesc.AddInputElementVertex("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
		psDesc.SetRasterizerState(FillMode::Solid, CullMode::Back);
		psDesc.SetPrimitiveTopologyType(PrimitiveTopologyType::Point);
		psDesc.AddRenderTargetState(BlendMode::Add, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		psDesc.SetSampleState(1, 0);
		pso.Create(directXDevice.GetDevice(), psDesc);
	}

	DirectXHelper::VertexBuffer vb;
	DirectXHelper::ConstantBuffer cb;
	struct TransformCB {
		Matrix44 worldMatrix;
		Matrix44 viewMatrix;
		Matrix44 projectionMatrix;
	};
	TransformCB transform;
	{
		std::vector<Vector4> vertices{
			{ -0.5f,  0.5f,  0.5f, 1.0f },
			{  0.5f,  0.5f,  0.5f, 1.0f },
			{ -0.5f, -0.5f,  0.5f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f },
			{  0.5f, -0.5f, -0.5f, 1.0f },
		};
		vb.Create(directXDevice.GetDevice(), vertices.size(), sizeof(vertices[0]));
		vb.WriteData(vertices.data());

		cb.Create(directXDevice.GetDevice(), sizeof(transform));
		float aspect = static_cast<float>(directXDevice.GetSwapChainWidth()) / static_cast<float>(directXDevice.GetSwapChainHeight());
		transform.projectionMatrix = Matrix44::MakePerspectiveProjection(ToRad(45.0f), aspect, 0.01f, 100.0f);
	}

	float scale = 0;
	float rotate = 0;

	// メインループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			directXDevice.StertScreenRendering();

			// 更新
			{
				scale += 0.001f;
				rotate += ToRad(1.0f);
				transform.worldMatrix = Matrix44::MakeAffine(Vector3{ scale }, Vector3{ rotate, rotate, rotate }, { 0.0f,0.0f,5.0f });
				transform.viewMatrix = Inverse(Matrix44::MakeAffine(Vector3{ 1.0f }, Vector3{ 0.0f }, { 0.0f,0.0f,-5.0f }));
				cb.WriteData(&transform);
			}

			// 描画
			{
				auto cmdList = directXDevice.GetCommnadList();

				cmdList->SetGraphicsRootSignature(rs.Get());
				cmdList->SetPipelineState(pso.Get());
				cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
				cmdList->IASetVertexBuffers(0, 1, &vb.GetView());
				cmdList->SetGraphicsRootConstantBufferView(0, cb.GetGPUAddress());
				cmdList->DrawInstanced(vb.GetVertexCount(), 1, 0, 0);
			}

			directXDevice.FinishScreenRendering();
		}
	}

	directXDevice.Finalize();
	window.Close();

	return 0;
}