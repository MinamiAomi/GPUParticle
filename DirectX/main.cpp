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
	
	DirectXHelper::GPUResource rwsb;
	DirectXHelper::Descriptor uav;
	DirectXHelper::ConstantBuffer targetCB;
	static const uint32_t kParticleCount = 10;
	struct TargetCB {
		Vector3 target;
	};
	struct Particle {
		Vector3 position;
		Vector3 velocity;
		Vector3 acceleration;
	};
	TargetCB target;
	{
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Particle) * kParticleCount);
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		rwsb.Create(directXDevice.GetDevice(), heapProperties, resourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, "ParticleBuffer");
		uav = directXDevice.GetCommonHeap().Allocate();
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = kParticleCount;
		uavDesc.Buffer.StructureByteStride = sizeof(Particle);
		directXDevice.GetDevice()->CreateUnorderedAccessView(rwsb.Get(), nullptr, &uavDesc, uav.cpu);

		target.target = { 0.0f,0.0f,20.0f };
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

		ID3D12DescriptorHeap* ppHeaps[] = { directXDevice.GetCommonHeap().Get() };
		cmdList->SetDescriptorHeaps(1, ppHeaps);
		cmdList->SetComputeRootSignature(crs.Get());
		cmdList->SetPipelineState(cpso.Get());
		cmdList->SetComputeRootDescriptorTable(0, uav.gpu);
		cmdList->Dispatch(kParticleCount, 1, 1);
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::UAV(rwsb.Get()),
			CD3DX12_RESOURCE_BARRIER::Transition(rwsb.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ)
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



	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = rwsb.GetGPUAddress();
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

	// メインループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			directXDevice.StertScreenRendering();
			auto cmdList = directXDevice.GetCommnadList();
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(rwsb.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				cmdList->ResourceBarrier(1, &barrier);
				cmdList->SetComputeRootSignature(crs.Get());
				cmdList->SetPipelineState(cpso.Get());
				cmdList->SetComputeRootDescriptorTable(0, uav.gpu);
				cmdList->SetComputeRootConstantBufferView(1, targetCB.GetGPUAddress());
				cmdList->Dispatch(kParticleCount, 1, 1);
				D3D12_RESOURCE_BARRIER barriers[] = {
					CD3DX12_RESOURCE_BARRIER::UAV(rwsb.Get()),
					CD3DX12_RESOURCE_BARRIER::Transition(rwsb.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ) 
				};
				cmdList->ResourceBarrier(_countof(barriers), barriers);

			}
			// 更新
			{
				transform.viewMatrix = Inverse(Matrix44::MakeAffine(Vector3{ 1.0f }, Vector3{ 0.0f }, { 0.0f,0.0f,-5.0f }));
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