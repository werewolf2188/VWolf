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

		Ref<Shader> shader = ShaderLibrary::GetShader(shaderName.c_str());
		frame = context->mCurrBackBuffer;

		MatrixFloat4x4 projection = m_camera->GetProjection();
		auto pso = ((HLSLShader*)shader.get())->GetPipeline();
		dx12ResetCommandList(context, pso);

		dx12SetCommandListClientArea(context);

		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		if (clearColor)
			dx12ClearRenderTargetView(context, { { { backgroundColor.r, backgroundColor.b, backgroundColor.g, backgroundColor.a } } });
		if (clearDepthStencil)
			dx12ClearDepthStencilView(context);
		// Specify the buffers we are going to render to.
		dx12SetRenderTarget(context);

		shader->Bind();

		for (int i = 0; i < items.size(); i++) {			
			groups[i]->Bind();
			context->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			shader->SetData(&projection, "Camera", sizeof(VWolf::MatrixFloat4x4), 0);
			shader->SetData(&items[i]->transform, "Object", sizeof(VWolf::MatrixFloat4x4), 0);
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