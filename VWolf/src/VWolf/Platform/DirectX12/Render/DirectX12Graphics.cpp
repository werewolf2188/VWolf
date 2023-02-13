#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Graphics.h"
#include "VWolf/Core/Render/RenderItem.h"

#include "VWolf/Core/Application.h"
#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

namespace VWolf {
	static DirectX12Context* GetMainContext() {
		return ((DirectX12Driver*)Application::GetApplication()->GetDriver())->GetContext();
	}

	//void DirectX12Renderer::ProcessItems()
	//{
	//	if (frame != context->mCurrBackBuffer) {
	//		dx12ResetCommandListAllocator(context);
	//		groups.clear();
	//	}
	//	
	//	for (auto renderItem : items) {
	//		auto data = renderItem->data.GetVertices();
	//		auto indices = renderItem->data.indices;
	//		Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(context, data.data(), data.size() * sizeof(float));
	//		vertices->SetLayout(MeshData::Layout);
	//		Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(context, indices.data(), indices.size());
	//		Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>(context);
	//		group->AddVertexBuffer(vertices);
	//		group->SetIndexBuffer(index);
	//		groups.push_back(group);
	//	}

	//	frame = context->mCurrBackBuffer;

 //       CameraPass projection = GetCameraPass();
	//	
	//	dx12ResetCommandList(context, nullptr);
	//	dx12SetCommandListClientArea(context);

	//	// Indicate a state transition on the resource usage.
	//	dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//	if (clearColor)
	//		dx12ClearRenderTargetView(context, { { { backgroundColor.r, backgroundColor.b, backgroundColor.g, backgroundColor.a } } });
	//	if (clearDepthStencil)
	//		dx12ClearDepthStencilView(context);
	//	// Specify the buffers we are going to render to.
	//	dx12SetRenderTarget(context);

	//	for (int i = 0; i < items.size(); i++) {	

	//		Ref<Shader> shader = ShaderLibrary::GetShader(items[i]->material.GetName().c_str());
	//		void* material = items[i]->material.GetDataPointer();
 //           Light* lights = this->lights.data();

	//		auto pso = ((HLSLShader*)shader.get())->GetPipeline();
	//		context->mCommandList->SetPipelineState(pso.Get());

	//		shader->Bind();

	//		groups[i]->Bind();
	//		context->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//		shader->SetData(&projection, ShaderLibrary::CameraBufferName, sizeof(CameraPass), i);
	//		shader->SetData(&items[i]->transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), i);
	//		shader->SetData(material, materialName.c_str(), items[i]->material.GetSize(), i);
	//		shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, i);
	//		uint32_t count = groups[i]->GetIndexBuffer()->GetCount();

	//		context->mCommandList->DrawIndexedInstanced(
	//			count,
	//			1, 0, 0, 0);
	//		//groups[i]->Unbind();			
	//	}

	//	dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	//	dx12ExecuteCommands(context);
	//}

	// TODO: Working as intended, but not happy with the implementation
	void DirectX12Graphics::DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera)
	{
		if (frame != GetMainContext()->mCurrBackBuffer) {
			groups.clear();
		}
		frame = GetMainContext()->mCurrBackBuffer;
		auto data = mesh.vertices;
		auto indices = mesh.indices;
		Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(GetMainContext(), data.data(), data.size() * sizeof(Vertex));
		vertices->SetLayout(MeshData::Layout);
		Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(GetMainContext(), indices.data(), indices.size());
		Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>(GetMainContext());
		group->AddVertexBuffer(vertices);
		group->SetIndexBuffer(index);
		groups.push_back(group);

		Camera* cam = camera != nullptr ? camera.get() : Camera::main;

		CameraPass cameraPass = {
			cam->GetViewMatrix(),
			inverse(cam->GetViewMatrix()),
			cam->GetProjection(),
			inverse(cam->GetProjection()),
			cam->GetViewProjection(),
			inverse(cam->GetViewProjection()),
			cam->GetPosition(),
			0,
			cam->GetDisplaySize(),
			{ 1 / cam->GetDisplaySize().x, 1 / cam->GetDisplaySize().y },
			cam->GetNearZ(),
			cam->GetFarZ(),
			Time::GetTotalTime(),
			Time::GetDeltaTime()
		};

		MatrixFloat4x4 transform = translate(MatrixFloat4x4(1.0f), Vector3Float(position));
		transform = VWolf::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
		transform = VWolf::rotate(transform, rotation.y, { 0.0f, 1.0f, 0.0f });
		transform = VWolf::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });

		dx12ResetCommandListAllocator(GetMainContext());
		dx12ResetCommandList(GetMainContext(), nullptr);
		dx12SetCommandListClientArea(GetMainContext());
		dx12ResourceBarrierTransitionForCurrentBackBuffer(GetMainContext(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		dx12SetRenderTarget(GetMainContext());

		Ref<Shader> shader = ShaderLibrary::GetShader(material.GetName().c_str());
		void* material1 = material.GetDataPointer();
		Light* lights = this->lights.data();

		auto pso = ((HLSLShader*)shader.get())->GetPipeline();
		GetMainContext()->mCommandList->SetPipelineState(pso.Get());

		shader->Bind();

		group->Bind();
		GetMainContext()->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
		shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
		shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
		shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
		uint32_t count = group->GetIndexBuffer()->GetCount();

		GetMainContext()->mCommandList->DrawIndexedInstanced(
			count,
			1, 0, 0, 0);

		dx12ResourceBarrierTransitionForCurrentBackBuffer(GetMainContext(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		dx12ExecuteCommands(GetMainContext());
		free(material1);
	}

	// TODO: Working as intended, but not happy with the implementation
	void DirectX12Graphics::RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera)
	{
		if (frame != GetMainContext()->mCurrBackBuffer) {
			groups.clear();
		}

		frame = GetMainContext()->mCurrBackBuffer;
		auto data = mesh.vertices;
		auto indices = mesh.indices;
		Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(GetMainContext(), data.data(), data.size() * sizeof(Vertex));
		vertices->SetLayout(MeshData::Layout);
		Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(GetMainContext(), indices.data(), indices.size());
		Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>(GetMainContext());
		group->AddVertexBuffer(vertices);
		group->SetIndexBuffer(index);
		groups.push_back(group);

		Camera* cam = camera != nullptr ? camera.get() : Camera::main;

		CameraPass cameraPass = {
			cam->GetViewMatrix(),
			inverse(cam->GetViewMatrix()),
			cam->GetProjection(),
			inverse(cam->GetProjection()),
			cam->GetViewProjection(),
			inverse(cam->GetViewProjection()),
			cam->GetPosition(),
			0,
			cam->GetDisplaySize(),
			{ 1 / cam->GetDisplaySize().x, 1 / cam->GetDisplaySize().y },
			cam->GetNearZ(),
			cam->GetFarZ(),
			Time::GetTotalTime(),
			Time::GetDeltaTime()
		};
		
		dx12ResetCommandListAllocator(GetMainContext());
		dx12ResetCommandList(GetMainContext(), nullptr);
		dx12SetCommandListClientArea(GetMainContext());
		dx12ResourceBarrierTransitionForCurrentBackBuffer(GetMainContext(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		dx12SetRenderTarget(GetMainContext());		

		Ref<Shader> shader = ShaderLibrary::GetShader(material.GetName().c_str());
		void* material1 = material.GetDataPointer();
		Light* lights = this->lights.data();

		auto pso = ((HLSLShader*)shader.get())->GetPipeline();
		GetMainContext()->mCommandList->SetPipelineState(pso.Get());

		shader->Bind();

		group->Bind();
		GetMainContext()->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
		shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
		shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
		shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
		uint32_t count = group->GetIndexBuffer()->GetCount();

		GetMainContext()->mCommandList->DrawIndexedInstanced(
			count,
			1, 0, 0, 0);

		dx12ResourceBarrierTransitionForCurrentBackBuffer(GetMainContext(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		dx12ExecuteCommands(GetMainContext());
		free(material1);		
	}

	// TODO: Rework to follow a consistent flow with the command list and command allocator
	void DirectX12Graphics::ClearColorImpl(Color color)
	{
		dx12ResetCommandList(GetMainContext(), nullptr);
		dx12SetCommandListClientArea(GetMainContext());
		dx12ResourceBarrierTransitionForCurrentBackBuffer(GetMainContext(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		dx12ClearRenderTargetView(GetMainContext(), {{{color.r, color.b, color.g, color.a}}});
		dx12ResourceBarrierTransitionForCurrentBackBuffer(GetMainContext(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		dx12ExecuteCommands(GetMainContext());
	}

	// TODO: Rework to follow a consistent flow with the command list and command allocator
	void DirectX12Graphics::ClearImpl()
	{
		dx12ResetCommandList(GetMainContext(), nullptr);
		dx12SetCommandListClientArea(GetMainContext());
		dx12ClearDepthStencilView(GetMainContext());
		dx12ExecuteCommands(GetMainContext());
		lights.clear();
	}

	void DirectX12Graphics::AddLightImpl(Light& light)
	{
		lights.push_back(light);
	}

	void DirectX12Graphics::DrawGridImpl()
	{
	}
}
#endif
