#include "vwpch.h"

#include "DirectX12RenderAPI.h"
#include "VWolf/Core/Render/Shader.h"
#include "HLSLShader.h"

#include "VWolf/Core/Render/Buffer.h"
#include "DirectX12Buffer.h"

#include "VWolf/Core/Render/BufferGroup.h"
#include "DirectX12BufferGroup.h"

#include "VWolf/Core/Render/UniformBuffer.h"
#include "DirectX12ConstantBuffer.h"


#ifdef VWOLF_PLATFORM_WINDOWS

namespace VWolf {
	DirectX12RenderAPI::DirectX12RenderAPI(HWND__* window, DirectX12Context* context): m_window(window), context(context)
	{
		Shader::SetDefaultCreateMethod([window, context](const std::string& name, BufferLayout layout) {
			return CreateRef<HLSLShader>(name, layout, window, context);
		});

		VertexBuffer::SetDefaultCreateSizeMethod([window, context](uint32_t size) {
			return CreateRef<DirectX12VertexBuffer>(window, context, size);
		});

		VertexBuffer::SetDefaultCreateDataAndSizeMethod([window, context](float* vertices, uint32_t size) {
			return CreateRef<DirectX12VertexBuffer>(window, context, vertices, size);
		});

		IndexBuffer::SetDefaultCreateMethod([window, context](uint32_t* indices, uint32_t count) {
			return CreateRef<DirectX12IndexBuffer>(window, context, indices, count);
		});

		BufferGroup::SetDefaultCreateMethod([window, context]() {
			return CreateRef<DirectX12BufferGroup>(window, context);
		});

		UniformBuffer::SetDefaultCreateMethod([window, context](Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding) {
			return CreateRef<DirectX12ConstantBuffer>(window, context, shader, name, size, binding);
		});
	}
	void DirectX12RenderAPI::Begin(Camera& camera, Ref<Shader> shader)
	{
		dx12ResetCommandListAllocator(context);
		auto pso = ((HLSLShader*)shader.get())->GetPipeline();
		dx12ResetCommandList(context, pso);

		dx12SetCommandListClientArea(context);

		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
	void DirectX12RenderAPI::ClearColor(Color color)
	{
		// Clear the back buffer and depth buffer.
		dx12ClearRenderTargetView(context, { { { color.r, color.b, color.g, color.a } } });
	}
	void DirectX12RenderAPI::Clear()
	{
		dx12ClearDepthStencilView(context);
		// Specify the buffers we are going to render to.
		dx12SetRenderTarget(context);
	}
	void DirectX12RenderAPI::End()
	{
		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		dx12ExecuteCommands(context);
	}
	void DirectX12RenderAPI::Resize(unsigned int m_Width, unsigned int m_Height)
	{
		if (m_Width == 0 && m_Height == 0) return;
		dx12ResizeBuffers(context, m_Width, m_Height);
	}
	void DirectX12RenderAPI::DrawIndexed(const Ref<BufferGroup>& group, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : group->GetIndexBuffer()->GetCount();
		context->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->mCommandList->DrawIndexedInstanced(
			count,
			1, 0, 0, 0);

	}
}

#endif