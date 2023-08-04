
#if 0
#include "Window.h"
#include "DirectXDevice.h"
#include "ShaderCompiler.h"
#include "MathUtils.h"
#include "Input.h"

using namespace Math;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Window window;
	window.Create();

	Input::Initalize(window.GetHWND());

	DirectXDevice directXDevice;
	directXDevice.Initalize(window.GetHWND());

	ShaderCompiler::Initalize();

	window.Show();
	
	DirectXHelper::GPUResource particlesBuffer;
	DirectXHelper::Descriptor particlesBufferView;
	DirectXHelper::ConstantBuffer targetCB;
	static const uint32_t kParticleCount = 65536;
	struct TargetCB {
		Vector3 target;
	};
	struct Particle {
		Vector4 position;
		Vector3 velocity;
		Vector3 acceleration;
	};
	TargetCB target;
	{
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Particle) * kParticleCount);
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		particlesBuffer.Create(directXDevice.GetDevice(), heapProperties, resourceDesc, D3D12_RESOURCE_STATE_COMMON, "ParticleBuffer");
		particlesBufferView = directXDevice.GetCommonHeap().Allocate();
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = kParticleCount;
		uavDesc.Buffer.StructureByteStride = sizeof(Particle);
		directXDevice.GetDevice()->CreateUnorderedAccessView(particlesBuffer.Get(), nullptr, &uavDesc, particlesBufferView.cpu);

		target.target = { 0.0f,0.0f,0.1f };
		targetCB.Create(directXDevice.GetDevice(), sizeof(TargetCB));
		targetCB.WriteData(&target);
	}

	DirectXHelper::RootSignature crs;
	DirectXHelper::PipelineState cpso;
	{
		DirectXHelper::RootSignatureDesc rsd;
		rsd.AddDescriptorRange(DirectXHelper::RootSignatureDesc::RangeType::UAV, 1, 0);
		rsd.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		crs.Create(directXDevice.GetDevice(), rsd);

		auto cs = ShaderCompiler::Compile(L"Resource/Shader/ParticleInitalize.CS.hlsl", L"main", L"cs_6_0");

		DirectXHelper::ComputePipelineStateDesc cpsod;
		cpsod.SetRootSignature(crs.Get());
		cpsod.SeComputeShader(cs->GetBufferPointer(), cs->GetBufferSize());
		cpso.Create(directXDevice.GetDevice(), cpsod);

		auto cmdList = directXDevice.GetCommnadList();

		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(particlesBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		cmdList->ResourceBarrier(1, &barrier);
		ID3D12DescriptorHeap* ppHeaps[] = { directXDevice.GetCommonHeap().Get() };
		cmdList->SetDescriptorHeaps(1, ppHeaps);
		cmdList->SetComputeRootSignature(crs.Get());
		cmdList->SetPipelineState(cpso.Get());
		cmdList->SetComputeRootDescriptorTable(0, particlesBufferView.gpu);
		cmdList->Dispatch(kParticleCount, 1, 1);
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::UAV(particlesBuffer.Get()),
			CD3DX12_RESOURCE_BARRIER::Transition(particlesBuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ)
		};
		cmdList->ResourceBarrier(_countof(barriers), barriers);
		directXDevice.SubmitCommandList();
		directXDevice.WaitForGPU();
		directXDevice.ResetCommandList(0);
	}
	{
		DirectXHelper::RootSignatureDesc rsd;
		rsd.AddDescriptorRange(DirectXHelper::RootSignatureDesc::RangeType::UAV, 1, 0);
		rsd.AddDescriptor(DirectXHelper::RootSignatureDesc::DescriptorType::CBV, 0);
		rsd.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		crs.Create(directXDevice.GetDevice(), rsd);

		auto cs = ShaderCompiler::Compile(L"Resource/Shader/ParticleUpdate.CS.hlsl", L"main", L"cs_6_0");

		DirectXHelper::ComputePipelineStateDesc cpsod;
		cpsod.SetRootSignature(crs.Get());
		cpsod.SeComputeShader(cs->GetBufferPointer(), cs->GetBufferSize());
		cpso.Create(directXDevice.GetDevice(), cpsod);
	}

	DirectXHelper::RootSignature rs;
	DirectXHelper::PipelineState pso;
	{
		using DescriptorType = DirectXHelper::RootSignatureDesc::DescriptorType;
		using RangeType = DirectXHelper::RootSignatureDesc::RangeType;
		using FillMode = DirectXHelper::GraphicsPipelineStateDesc::FillMode;
		using CullMode = DirectXHelper::GraphicsPipelineStateDesc::CullMode;
		using PrimitiveTopologyType = DirectXHelper::GraphicsPipelineStateDesc::PrimitiveTopology;
		using BlendMode = DirectXHelper::GraphicsPipelineStateDesc::BlendMode;
		using DepthWriteMask = DirectXHelper::GraphicsPipelineStateDesc::DepthWriteMask;
		using ComparisonFunc = DirectXHelper::GraphicsPipelineStateDesc::ComparisonFunc;


		DirectXHelper::RootSignatureDesc rsDesc;
		rsDesc.AddDescriptor(DescriptorType::CBV, 0);
		rsDesc.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rs.Create(directXDevice.GetDevice(), rsDesc);

		auto vs = ShaderCompiler::Compile(L"Resource/Shader/ParticleGraphics_VS.hlsl", L"main", L"vs_6_0");
		auto ps = ShaderCompiler::Compile(L"Resource/Shader/ParticleGraphics_PS.hlsl", L"main", L"ps_6_0");
		auto gs = ShaderCompiler::Compile(L"Resource/Shader/ParticleGraphics_GS.hlsl", L"main", L"gs_6_0");


		DirectXHelper::GraphicsPipelineStateDesc psDesc;
		psDesc.SetRootSignature(rs.Get());
		psDesc.SetVertexShader(vs->GetBufferPointer(), vs->GetBufferSize());
		psDesc.SetPixelShader(ps->GetBufferPointer(), ps->GetBufferSize());
		psDesc.SetGeometryShader(gs->GetBufferPointer(), gs->GetBufferSize());
		psDesc.AddInputElementVertex("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
		psDesc.SetRasterizerState(FillMode::Solid, CullMode::Back);
		psDesc.SetPrimitiveTopologyType(PrimitiveTopology::Point);
		psDesc.AddRenderTargetState(BlendMode::Add, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		psDesc.SetSampleState(1, 0);
		pso.Create(directXDevice.GetDevice(), psDesc);
	}



	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = particlesBuffer.GetGPUAddress();
	vbv.SizeInBytes = sizeof(Particle) * kParticleCount;
	vbv.StrideInBytes = sizeof(Particle);
	DirectXHelper::VertexBuffer vb;
	DirectXHelper::ConstantBuffer cb;
	struct TransformCB {
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
		vb.Create(directXDevice.GetDevice(), uint32_t(vertices.size()), sizeof(vertices[0]));
		vb.WriteData(vertices.data());

		cb.Create(directXDevice.GetDevice(), sizeof(transform));
		float aspect = static_cast<float>(directXDevice.GetSwapChainWidth()) / static_cast<float>(directXDevice.GetSwapChainHeight());
		transform.projectionMatrix = Matrix44::MakePerspectiveProjection(ToRad(45.0f), aspect, 0.01f, 100.0f);
	}
	Vector3 position{ 0.0f,0.0f,-10.0f };
	Vector3 euler{};
	Quaternion rotate = Quaternion::MakeLookRotation(target.target - position);

	// メインループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			directXDevice.BeginFrame();
			directXDevice.StertScreenRendering();
			auto cmdList = directXDevice.GetCommnadList();
			{

				ImGui::Begin("Window");
				ImGui::DragFloat3("pos", &target.target.x, 0.01f);
				targetCB.WriteData(&target);
				

				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(particlesBuffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				cmdList->ResourceBarrier(1, &barrier);
				cmdList->SetComputeRootSignature(crs.Get());
				cmdList->SetPipelineState(cpso.Get());
				cmdList->SetComputeRootDescriptorTable(0, particlesBufferView.gpu);
				cmdList->SetComputeRootConstantBufferView(1, targetCB.GetGPUAddress());
				cmdList->Dispatch(kParticleCount / 16, 1, 1);
				D3D12_RESOURCE_BARRIER barriers[] = {
					CD3DX12_RESOURCE_BARRIER::UAV(particlesBuffer.Get()),
					CD3DX12_RESOURCE_BARRIER::Transition(particlesBuffer.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ) 
				};
				cmdList->ResourceBarrier(_countof(barriers), barriers);

			}
			// 更新
			{
				ImGui::DragFloat3("Camera pos", &position.x, 0.1f);
				ImGui::DragFloat3("CameraRotate", &euler.x, 0.01f, 0.0f, Math::Pi * 2.0f);
				rotate = Quaternion::MakeFromEulerAngle(euler);
				ImGui::End();
				
				transform.viewMatrix = Inverse(Matrix44::MakeAffine(Vector3{ 1.0f }, rotate, position));
				cb.WriteData(&transform);
			}

			// 描画
			{

				cmdList->SetGraphicsRootSignature(rs.Get());
				cmdList->SetPipelineState(pso.Get());
				cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
				//auto view = vb.GetView();
				cmdList->IASetVertexBuffers(0, 1, &vbv);
				cmdList->SetGraphicsRootConstantBufferView(0, cb.GetGPUAddress());
				cmdList->DrawInstanced(kParticleCount, 1, 0, 0);
			}
			directXDevice.FinishScreenRendering();
		}
	}

	directXDevice.Finalize();
	window.Close();

	return 0;
}

#else

#include "Window.h"
#include "Graphics.h"
#include "CommandList.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Engine::graphics.Initalize();

	


	return 0;
}


#endif