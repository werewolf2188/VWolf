#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Graphics.h"
#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

#include "VWolf/Platform/DirectX12/Core/DX12Command.h"
#include "VWolf/Platform/DirectX12/Core/DX12Surface.h"
#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"

namespace VWolf {
	void DirectX12Graphics::Initialize() {
		shadowMap = CreateRef<DirectX12RenderTexture>(1024, 1024, true, TextureOptions()); // TODO: This fails for 1024x1024
		emptyShadowMap = CreateRef<DirectX12Texture2D>(TextureDefault::White, 1024, 1024, TextureOptions());
	}
	// TODO: Working as intended, but not happy with the implementation
	// TODO: Better names. This is for immediate rendering
	void DirectX12Graphics::DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera)
	{
		auto data = mesh.vertices;
		if (data.size() == 1) return; // It's a light
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

		MatrixFloat4x4 transform = translate(MatrixFloat4x4(1.0f), Vector3Float(position));
		transform = VWolf::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
		transform = VWolf::rotate(transform, rotation.y, { 0.0f, 1.0f, 0.0f });
		transform = VWolf::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });

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
		std::vector<MatrixFloat4x4> spaces;
		for (int i = 0; i < this->lights.size(); i++) {
			spaces.push_back(lights[i].GetLightSpaceMatrix());
		}
		MatrixFloat4x4* spacesPointer = spaces.data();
		shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(MatrixFloat4x4) * Light::LightsMax, 0);
		// Adding textures
		for (auto textureInput : shader->GetTextureInputs()) {
			if (textureInput.GetName() == "Shadow")
			{
				D3D12_GPU_DESCRIPTOR_HANDLE handle;
				handle.ptr = (UINT64)emptyShadowMap->GetHandler();
				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootDescriptorTable(textureInput.GetIndex(), handle);
			}
			else
			{
				D3D12_GPU_DESCRIPTOR_HANDLE handle;
				handle.ptr = (UINT64)material.GetTexture(textureInput.GetName())->GetHandler();
				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootDescriptorTable(textureInput.GetIndex(), handle);
			}
		}
		//
		uint32_t count = indices.size();

		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->DrawIndexedInstanced(
			count,
			1, 0, 0, 0);
		free(material1);
		shapes++;
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
		spaces.push_back(light.GetLightSpaceMatrix());
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
		spaces.clear();
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
		if (renderTexture) {
			((DirectX12RenderTexture*)renderTexture.get())->Transition(D3D12_RESOURCE_STATE_RENDER_TARGET);
			((DirectX12RenderTexture*)renderTexture.get())->Bind();
		}
		
	}

	void DirectX12Graphics::EndSceneImpl()
	{
		shadowMap->Transition(D3D12_RESOURCE_STATE_DEPTH_WRITE);

		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->ClearDepthStencilView(shadowMap->GetTexture()->GetHandle().GetCPUAddress(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		shadowMap->Bind();
		DrawShadowMap();
		shadowMap->Transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		if (renderTexture) {
			((DirectX12RenderTexture*)renderTexture.get())->Bind();
		}
		else {
			auto rtv = DirectX12Driver::GetCurrent()->GetSurface()->GetCurrentRenderTargetView();
			rtv->TransitionResource(DirectX12Driver::GetCurrent()->GetCommands(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
				->OMSetRenderTargets(1, &rtv->GetHandle().GetCPUAddress(), FALSE, &DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress());

			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->RSSetViewports(1, &DirectX12Driver::GetCurrent()->GetSurface()->GetScreenViewport());
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->RSSetScissorRects(1, &DirectX12Driver::GetCurrent()->GetSurface()->GetScissorRect());
		}

		DrawQueue();
		DrawPostProcess();

		auto rtv = DirectX12Driver::GetCurrent()->GetSurface()->GetCurrentRenderTargetView();

		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->OMSetRenderTargets(1, &rtv->GetHandle().GetCPUAddress(), FALSE, &DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress());
		if (renderTexture) {
			((DirectX12RenderTexture*)renderTexture.get())->Transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
	}

	void DirectX12Graphics::DrawShadowMap()
	{
		for (Light& light : lights) {
			int shadowShapes = shapes;
			MatrixFloat4x4 viewProjection = light.GetLightSpaceMatrix();

			for (Ref<RenderItem> item : items) {

				auto& mesh = item->data;
				auto& material = item->material;
				MatrixFloat4x4 transform = item->transform;

				auto data = mesh.vertices;
				if (data.size() == 1) continue;; // It's a light
				auto indices = mesh.indices;
				Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), data.data(), data.size() * sizeof(Vertex));
				Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), indices.data(), indices.size());
				Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>();
				group->SetVertexBuffer(vertices);
				group->SetIndexBuffer(index);

				groups.emplace_back(DirectX12Driver::GetCurrent()->GetCommands()->GetCurrentFence(), group);

				vertices->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());
				index->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());

				Ref<Shader> shader = ShaderLibrary::GetShader("Shadow");

				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				group->Bind(DirectX12Driver::GetCurrent()->GetCommands());
				auto pso = ((HLSLShader*)shader.get())->GetPipeline();
				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetPipelineState(pso.Get());
				shader->Bind();

				shader->SetData(&viewProjection, ShaderLibrary::CameraBufferName, sizeof(MatrixFloat4x4), shadowShapes);
				shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), shadowShapes);
				void* material1 = material.GetDataPointer();
				shader->SetData(material1, materialName.c_str(), material.GetSize(), shapes);

				uint32_t count = indices.size();

				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->DrawIndexedInstanced(
					count,
					1, 0, 0, 0);
				free(material1);
				shadowShapes++;
			}
		}
	}

	void DirectX12Graphics::DrawQueue()
	{
		for (Ref<RenderItem> item : items) {
			auto& mesh = item->data;
			auto& material = item->material;
			Ref<Camera> camera = item->camera;
			MatrixFloat4x4 transform = item->transform;

			auto data = mesh.vertices;
			if (data.size() == 1) continue;; // It's a light
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
			MatrixFloat4x4* spacesPointer = spaces.data();

			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			group->Bind(DirectX12Driver::GetCurrent()->GetCommands());
			auto pso = ((HLSLShader*)shader.get())->GetPipeline();
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetPipelineState(pso.Get());
			shader->Bind();
			shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), shapes);
			shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), shapes);
			shader->SetData(material1, materialName.c_str(), material.GetSize(), shapes);
			shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, shapes);			
			shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(MatrixFloat4x4) * Light::LightsMax, shapes);
			// Adding textures
			for (auto textureInput : shader->GetTextureInputs()) {
				if (textureInput.GetName() == "Shadow") 
				{
					D3D12_GPU_DESCRIPTOR_HANDLE handle;
					handle.ptr = (UINT64)shadowMap->GetHandler();
					DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootDescriptorTable(textureInput.GetIndex(), handle);
				}
				else 
				{
					D3D12_GPU_DESCRIPTOR_HANDLE handle;
					handle.ptr = (UINT64)material.GetTexture(textureInput.GetName())->GetHandler();
					DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetGraphicsRootDescriptorTable(textureInput.GetIndex(), handle);
				}				
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
