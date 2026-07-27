#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Graphics.h"
#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Core/Render/GraphicsContext.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

#include "VWolf/Platform/DirectX12/Core/DX12Command.h"
#include "VWolf/Platform/DirectX12/Core/DX12Surface.h"
#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"

namespace VWolf {
	static D3D_PRIMITIVE_TOPOLOGY GetTopology(Topology topology) {
		switch (topology) {
		case Topology::Triangles:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case Topology::Quads:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case Topology::Lines:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case Topology::LinesStrip:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case Topology::Points:
			return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		default: return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}

	void DirectX12Graphics::Initialize() {
		shadowMap = CreateRef<DirectX12RenderTexture>(1024, 1024, true, TextureOptions()); // TODO: This fails for 1024x1024
		emptyShadowMap = std::dynamic_pointer_cast<DirectX12Texture2D>(CreateRef<Texture2D>(UUID::NewUUID(), TextureDefault::White, 1024, 1024, TextureOptions())->GetInnerTexture());
	}

	void DirectX12Graphics::ClearColorImpl(Color color)
	{
		auto rtv = DirectX12Driver::GetCurrent()->GetSurface()->GetCurrentRenderTargetView();
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->ClearRenderTargetView(rtv->GetHandle().GetCPUAddress(), &color.GetR(), 0, nullptr);
		if (renderTexture) {
			auto directX12Rtv = (DirectX12RenderTexture*)renderTexture.get();
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->ClearRenderTargetView(directX12Rtv->GetTexture()->GetHandle().GetCPUAddress(), &color.GetR(), 0, nullptr);
		}
	}

	void DirectX12Graphics::ClearImpl()
	{
		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->ClearDepthStencilView(DirectX12Driver::GetCurrent()->GetDepthStencilBuffer()->GetHandle().GetCPUAddress(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
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

        ClearImpl();
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

		while (!textureGroups.empty())
		{
			auto& FirstObj = textureGroups.front();
			// GPU must have been idled when ForceRelease == true 
			if (FirstObj.first < NumCompletedCmdLists || forceRelease)
				textureGroups.pop_front();
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
	}

	void DirectX12Graphics::SetRenderTextureImpl(Ref<RenderTexture> renderTexture)
	{
		this->renderTexture = renderTexture->GetInnerTexture();
	}

	void DirectX12Graphics::BeginSceneImpl()
	{	
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
		Ref<PShader> shader = Shader::GetShader("Shadow")->GetInternalShader();
		for (const Matrix4x4& viewProjection : GraphicsContext::GetLightsSpaces()) {
			int shadowShapes = 0;

			for (Ref<GraphicsCommand> command : GraphicsContext::GetList().GetCommands()) {
				Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
				if (!drawMeshCommand->GetCastShadows()) continue;

				Ref<Mesh> mesh1 = drawMeshCommand->GetMesh();
                if (mesh1 == nullptr || mesh1->GetVertices().size() == 1) return;; // It's a light
				Ref<Material> material = drawMeshCommand->GetMaterial();
				Matrix4x4 transform = drawMeshCommand->GetTransform();

				mesh1->BuildVertexBuffer(shader->GetAttributes());

				auto data = mesh1->GetNativeVector();
				if (data.size() == 1) continue;; // It's a light
				auto indices = mesh1->GetTriangles();
				Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), data.data(), data.size() * sizeof(float), shader->GetAttributes());
				Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), indices.data(), indices.size());
				Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>();
				group->SetVertexBuffer(vertices);
				group->SetIndexBuffer(index);

				groups.emplace_back(DirectX12Driver::GetCurrent()->GetCommands()->GetCurrentFence(), group);

				vertices->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());
				index->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());

				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->IASetPrimitiveTopology(GetTopology(mesh1->GetSubMesh(0).GetTopology()));
				group->Bind(DirectX12Driver::GetCurrent()->GetCommands());
				auto pso = ((HLSLShader*)shader.get())->GetPipeline();
				DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetPipelineState(pso.Get());
				shader->Bind();

				shader->SetData(&viewProjection, Shader::CameraBufferName, sizeof(Matrix4x4), shadowShapes);
				shader->SetData(&transform, Shader::ObjectBufferName, sizeof(Matrix4x4), shadowShapes);
				void* material1 = material->GetDataPointer();
				shader->SetData(material1, materialName.c_str(), material->GetSize(), shadowShapes);

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
		int shapes = 0;
		for (Ref<GraphicsCommand> command : GraphicsContext::GetList().GetCommands()) {
			Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
			Ref<Mesh> mesh1 = drawMeshCommand->GetMesh();
			Ref<Material> material = drawMeshCommand->GetMaterial();
			Ref<Camera> camera = drawMeshCommand->GetCamera();
			Matrix4x4 transform = drawMeshCommand->GetTransform();

			if (mesh1 == nullptr || mesh1->GetVertices().size() == 1) continue; // It's a light

			Camera* cam = camera != nullptr ? camera.get() : Camera::main;

			CameraPass cameraPass = {
				cam->GetViewMatrix(),
				cam->GetViewMatrix().GetInverse(),
				cam->GetProjection(),
				cam->GetProjection().GetInverse(),
				cam->GetViewProjection(),
				cam->GetViewProjection().GetInverse(),
				cam->GetPosition(),
				0,
				cam->GetDisplaySize(),
				{ 1 / cam->GetDisplaySize().GetX(), 1 / cam->GetDisplaySize().GetY() },
				cam->GetNearZ(),
				cam->GetFarZ(),
				Time::GetTotalTime(),
				Time::GetDeltaTime()
			};

			Ref<PShader> shader = Shader::GetShader(material->GetShaderName().c_str())->GetInternalShader();

			mesh1->BuildVertexBuffer(shader->GetAttributes());
			Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>();

			auto data = mesh1->GetNativeVector();
			auto indices = mesh1->GetTriangles();
			Ref<DirectX12VertexBuffer> vertices = nullptr;
			if (data.size() > 1) {
				vertices = CreateRef<DirectX12VertexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), data.data(), data.size() * sizeof(float), shader->GetAttributes());
				group->SetVertexBuffer(vertices);
			}
			Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(DirectX12Driver::GetCurrent()->GetDevice(), indices.data(), indices.size());
			group->SetIndexBuffer(index);

			groups.emplace_back(DirectX12Driver::GetCurrent()->GetCommands()->GetCurrentFence(), group);

			if (vertices != nullptr) {
				vertices->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());
			}
			index->CopyToDefaultBuffer(DirectX12Driver::GetCurrent()->GetCommands());

			void* material1 = material->GetDataPointer();
			if (GraphicsContext::GetLights().size() == 0) {
				GraphicsContext::GetLights().push_back(Light());
			}
			Light* lights = GraphicsContext::GetLights().data();
			Matrix4x4* spacesPointer = GraphicsContext::GetLightsSpaces().data();

			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->IASetPrimitiveTopology(GetTopology(mesh1->GetSubMesh(0).GetTopology()));
			group->Bind(DirectX12Driver::GetCurrent()->GetCommands());
			auto pso = ((HLSLShader*)shader.get())->GetPipeline();
			DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()->SetPipelineState(pso.Get());
			shader->Bind();
			shader->SetData(&cameraPass, Shader::CameraBufferName, sizeof(CameraPass), shapes);
			shader->SetData(&transform, Shader::ObjectBufferName, sizeof(Matrix4x4), shapes);
			shader->SetData(material1, materialName.c_str(), material->GetSize(), shapes);
			shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, shapes);			
			shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(Matrix4x4) * Light::LightsMax, shapes);
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
					Ref<Texture> texture = material->GetTexture(textureInput.GetName());
					textureGroups.emplace_back(DirectX12Driver::GetCurrent()->GetCommands()->GetCurrentFence(), texture);
					handle.ptr = (UINT64)texture->GetHandler();
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
