#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS

#include "DirectX12RenderAPI.h"
#include "VWolf/Core/Render/Shader.h"
#include "HLSLShader.h"

#include "VWolf/Core/Render/Buffer.h"
#include "DirectX12Buffer.h"

#include "VWolf/Core/Render/BufferGroup.h"
#include "DirectX12BufferGroup.h"

namespace VWolf {
	DirectX12RenderAPI::DirectX12RenderAPI(HWND__* window, DirectX12Context* context): m_window(window), m_context(context)
	{
		Shader::SetDefaultCreateMethod([this](const char* name,
			ShaderSource vertexShader,
			BufferLayout layout,
			std::initializer_list<ShaderSource> otherShaders,
			std::initializer_list<ShaderParameter> parameters,
			ShaderConfiguration configuration) {
			return CreateRef<HLSLShader>(m_window, m_context, name, vertexShader, layout, otherShaders, parameters, configuration);
		});

		VertexBuffer::SetDefaultCreateSizeMethod([this](uint32_t size) {
			return CreateRef<DirectX12VertexBuffer>(m_window, m_context, size);
		});

		VertexBuffer::SetDefaultCreateDataAndSizeMethod([this](float* vertices, uint32_t size) {
			return CreateRef<DirectX12VertexBuffer>(m_window, m_context, vertices, size);
		});

		IndexBuffer::SetDefaultCreateMethod([this](uint32_t* indices, uint32_t count) {
			return CreateRef<DirectX12IndexBuffer>(m_window, m_context, indices, count);
		});

		BufferGroup::SetDefaultCreateMethod([this]() {
			return CreateRef<DirectX12BufferGroup>(m_window, m_context);
		});
	}
	void DirectX12RenderAPI::Begin(Camera& camera, Ref<Shader> shader)
	{
		dx12ResetCommandListAllocator(m_context);
		auto pso = ((HLSLShader*)shader.get())->GetPipeline();
		dx12ResetCommandList(m_context, pso);

		dx12SetCommandListClientArea(m_context);

		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(m_context, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
	void DirectX12RenderAPI::ClearColor(Color color)
	{
		// Clear the back buffer and depth buffer.
		dx12ClearRenderTargetView(m_context, { { { color.r, color.b, color.g, color.a } } });
	}
	void DirectX12RenderAPI::Clear()
	{
		dx12ClearDepthStencilView(m_context);
		// Specify the buffers we are going to render to.
		dx12SetRenderTarget(m_context);
	}
	void DirectX12RenderAPI::End()
	{
		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(m_context, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		dx12ExecuteCommands(m_context);
	}
	void DirectX12RenderAPI::Resize(unsigned int m_Width, unsigned int m_Height)
	{
		if (m_Width == 0 && m_Height == 0) return;
		dx12ResizeBuffers(m_context, m_Width, m_Height);
	}
	void DirectX12RenderAPI::DrawIndexed(const Ref<BufferGroup>& group, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : group->GetIndexBuffer()->GetCount();
		m_context->mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_context->mCommandList->DrawIndexedInstanced(
			count,
			1, 0, 0, 0);

	}
}

#endif