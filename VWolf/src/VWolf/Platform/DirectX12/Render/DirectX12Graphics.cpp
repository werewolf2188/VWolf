#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Graphics.h"
#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

#include "VWolf/Platform/DirectX12/Render/DirectX12Texture.h"

#include "VWolf/Platform/DirectX12/Core/DX12Command.h"
#include "VWolf/Platform/DirectX12/Core/DX12Surface.h"
#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"

namespace VWolf {

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
	// TODO: Better names. This is for immediate rendering
	void DirectX12Graphics::DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera)
	{
		/*if (frame != DirectX12Driver::GetCurrent()->GetContext()->mCurrBackBuffer) {
			groups.clear();
		}
		frame = DirectX12Driver::GetCurrent()->GetContext()->mCurrBackBuffer;
		auto data = mesh.vertices;
		auto indices = mesh.indices;
		Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(DirectX12Driver::GetCurrent()->GetContext(), data.data(), data.size() * sizeof(Vertex));
		vertices->SetLayout(MeshData::Layout);
		Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(DirectX12Driver::GetCurrent()->GetContext(), indices.data(), indices.size());
		Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>(DirectX12Driver::GetCurrent()->GetContext());
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

		dx12ResetCommandListAllocator(DirectX12Driver::GetCurrent()->GetContext());
		dx12ResetCommandList(DirectX12Driver::GetCurrent()->GetContext(), nullptr);
		dx12SetCommandListClientArea(DirectX12Driver::GetCurrent()->GetContext());
		dx12ResourceBarrierTransitionForCurrentBackBuffer(DirectX12Driver::GetCurrent()->GetContext(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		dx12SetRenderTarget(DirectX12Driver::GetCurrent()->GetContext());

		Ref<Shader> shader = ShaderLibrary::GetShader(material.GetName().c_str());
		void* material1 = material.GetDataPointer();
		Light* lights = this->lights.data();

		auto pso = ((HLSLShader*)shader.get())->GetPipeline();
		DirectX12Driver::GetCurrent()->GetContext()->mCommandList->SetPipelineState(pso.Get());

		shader->Bind();

		group->Bind();
		DirectX12Driver::GetCurrent()->GetContext()->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
		shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
		shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
		shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
		uint32_t count = group->GetIndexBuffer()->GetCount();

		DirectX12Driver::GetCurrent()->GetContext()->mCommandList->DrawIndexedInstanced(
			count,
			1, 0, 0, 0);

		dx12ResourceBarrierTransitionForCurrentBackBuffer(DirectX12Driver::GetCurrent()->GetContext(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		dx12ExecuteCommands(DirectX12Driver::GetCurrent()->GetContext());
		free(material1);*/
	}

	// TODO: Working as intended, but not happy with the implementation
	// TODO: Better names. This is for lazy rendering
	void DirectX12Graphics::RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera)
	{
		items.push_back(CreateRef<RenderItem>(mesh, material, transform, camera));		
	}

	void DirectX12Graphics::ClearColorImpl(Color color)
	{
		auto rtv = DirectX12Driver::GetCurrent()->GetSurface()->GetCurrentRenderTargetView();
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->ClearRenderTargetView(rtv->GetHandle().GetCPUAddress(), value_ptr(color), 0, nullptr);
		if (renderTexture) {
			auto directX12Rtv = (DirectX12RenderTexture*)renderTexture.get();
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->ClearRenderTargetView(directX12Rtv->GetTexture()->GetHandle().GetCPUAddress(), value_ptr(color), 0, nullptr);
		}
	}

	void DirectX12Graphics::ClearImpl()
	{
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->ClearDepthStencilView(DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}

	void DirectX12Graphics::AddLightImpl(Light& light)
	{
		lights.push_back(light);
	}

	void DirectX12Graphics::BindToRenderTexture() {
		auto rtv = (DirectX12RenderTexture*)renderTexture.get();

		rtv->Bind();
	}

	void DirectX12Graphics::UnbindToRenderTexture() {

		auto rtv = DirectX12Driver::GetCurrent()->GetSurface()->GetCurrentRenderTargetView();

		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->OMSetRenderTargets(1, &rtv->GetHandle().GetCPUAddress(), FALSE, &DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress());
	}

	void DirectX12Graphics::BeginFrameImpl()
	{		
		// Reset command list and allocator
		DirectX12Driver::GetCurrent()->GetCommands()->BeginFrame();

		// Set the viewport and rect
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->RSSetViewports(1, &DirectX12Driver::GetCurrent()->GetSurface()->GetScreenViewport());
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->RSSetScissorRects(1, &DirectX12Driver::GetCurrent()->GetSurface()->GetScissorRect());

		auto rtv = DirectX12Driver::GetCurrent()->GetSurface()->GetCurrentRenderTargetView();
		rtv->TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->OMSetRenderTargets(1, &rtv->GetHandle().GetCPUAddress(), FALSE, &DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress());

		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->SetDescriptorHeaps(1, DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap()->GetHeap().GetAddressOf());

		lights.clear();
		shapes = 0;
	}

	void DirectX12Graphics::ClearResources(bool forceRelease) {
		auto NumCompletedCmdLists = DirectX12Driver::GetCurrent()->GetCommands()->GetCompletedFence();
		// Release all objects whose cmd list number value < number of completed cmd lists
		while (!groups.empty())
		{
			auto& FirstObj = groups.front();
			// GPU must have been idled when ForceRelease == true 
			if (FirstObj.first < NumCompletedCmdLists || forceRelease)
				groups.pop_front();
			else
				break;
		}
	}

	void DirectX12Graphics::EndFrameImpl()
	{
		auto rtv = DirectX12Driver::GetCurrent()->GetSurface()->GetCurrentRenderTargetView();
		rtv->TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		DirectX12Driver::GetCurrent()->GetCommands()->EndFrame(DirectX12Driver::GetCurrent()->GetSurface());

		ClearResources(false);

		frame++;
	}

	void DirectX12Graphics::SetRenderTextureImpl(Ref<RenderTexture> renderTexture)
	{
		this->renderTexture = renderTexture;
	}

	void DirectX12Graphics::BeginSceneImpl()
	{
		items.clear();
		BindToRenderTexture();
	}

	void DirectX12Graphics::EndSceneImpl()
	{
		DrawShadowMap();
		DrawQueue();
		DrawPostProcess();
		UnbindToRenderTexture();
	}

	void DirectX12Graphics::DrawShadowMap()
	{
	}

	void DirectX12Graphics::DrawQueue()
	{
		for (Ref<RenderItem> item : items) {
			auto& mesh = item->data;
			auto& material = item->material;
			Ref<Camera> camera = item->camera;
			MatrixFloat4x4 transform = item->transform;

			auto data = mesh.vertices;
			auto indices = mesh.indices;
			Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), data.data(), data.size() * sizeof(Vertex));
			Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), indices.data(), indices.size());
			Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>();
			group->SetVertexBuffer(vertices);
			group->SetIndexBuffer(index);

			groups.emplace_back(DirectX12Driver::GetCurrent()->GetCommands()->GetCurrentFence(), group);

			vertices->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());
			index->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());

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

			Ref<Shader> shader = ShaderLibrary::GetShader(material.GetName().c_str());
			void* material1 = material.GetDataPointer();
			if (this->lights.size() == 0) {
				this->lights.push_back(Light());
			}
			Light* lights = this->lights.data();

			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			group->Bind(DirectX12Driver::GetCurrent()->GetCommands());
			auto pso = ((HLSLShader*)shader.get())->GetPipeline();
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetPipelineState(pso.Get());
			shader->Bind();

			shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), shapes);
			shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), shapes);
			shader->SetData(material1, materialName.c_str(), material.GetSize(), shapes);
			shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, shapes);
			// Adding textures
			for (auto textureInput : shader->GetTextureInputs()) {
				D3D12_GPU_DESCRIPTOR_HANDLE handle;
				handle.ptr = (UINT64)material.GetTexture(textureInput.GetName())->GetHandler();
				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootDescriptorTable(textureInput.GetIndex(), handle);
			}
			//
			uint32_t count = indices.size();

			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->DrawIndexedInstanced(
				count,
				1, 0, 0, 0);
			free(material1);
			shapes++;
		}
	}

	void DirectX12Graphics::DrawPostProcess()
	{
	}
}
#endif
