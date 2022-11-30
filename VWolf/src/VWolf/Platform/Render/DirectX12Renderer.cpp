#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Renderer.h"
#include "VWolf/Core/Render/RenderItem.h"

namespace VWolf {

	DirectX12Renderer::DirectX12Renderer(HWND__* window, DirectX12Context* context): Renderer(), window(window), context(context)
	{
	}

	void DirectX12Renderer::ProcessItems()
	{
		if (frame != context->mCurrBackBuffer) {
			dx12ResetCommandListAllocator(context);
			groups.clear();
		}
		
		for (auto renderItem : items) {
			auto data = renderItem->data.GetVertices();
			auto indices = renderItem->data.indices;
			Ref<DirectX12VertexBuffer> vertices = CreateRef<DirectX12VertexBuffer>(window, context, data.data(), data.size() * sizeof(float));
			vertices->SetLayout(MeshData::Layout);
			Ref<DirectX12IndexBuffer> index = CreateRef<DirectX12IndexBuffer>(window, context, indices.data(), indices.size());
			Ref<DirectX12BufferGroup> group = CreateRef<DirectX12BufferGroup>(window, context);
			group->AddVertexBuffer(vertices);
			group->SetIndexBuffer(index);
			groups.push_back(group);
		}

		frame = context->mCurrBackBuffer;

        CameraPass projection = GetCameraPass();
		
		dx12ResetCommandList(context, nullptr);
		dx12SetCommandListClientArea(context);

		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		if (clearColor)
			dx12ClearRenderTargetView(context, { { { backgroundColor.r, backgroundColor.b, backgroundColor.g, backgroundColor.a } } });
		if (clearDepthStencil)
			dx12ClearDepthStencilView(context);
		// Specify the buffers we are going to render to.
		dx12SetRenderTarget(context);

		for (int i = 0; i < items.size(); i++) {	

			Ref<Shader> shader = ShaderLibrary::GetShader(items[i]->material.GetShader().c_str());
			void* material = items[i]->material.GetDataPointer();
            Light* lights = this->lights.data();

			auto pso = ((HLSLShader*)shader.get())->GetPipeline();
			context->mCommandList->SetPipelineState(pso.Get());

			shader->Bind();

			groups[i]->Bind();
			context->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			shader->SetData(&projection, ShaderLibrary::CameraBufferName, sizeof(CameraPass), i);
			shader->SetData(&items[i]->transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), i);
			shader->SetData(material, items[i]->material.GetName(), items[i]->material.GetSize(), i);
			shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, i);
			uint32_t count = groups[i]->GetIndexBuffer()->GetCount();

			context->mCommandList->DrawIndexedInstanced(
				count,
				1, 0, 0, 0);
			//groups[i]->Unbind();			
		}

		dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		dx12ExecuteCommands(context);
	}
}
#endif
