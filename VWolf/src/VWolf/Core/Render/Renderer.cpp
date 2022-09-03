#include "vwpch.h"

#include "Renderer.h"

namespace VWolf {
	Scope<RenderAPI> Renderer::m_renderApi = nullptr;

	void Renderer::Begin(Camera& camera, Ref<Shader> shader)
	{		
		m_renderApi->Begin(camera, shader);
	}
	void Renderer::ClearColor(Color color)
	{
		m_renderApi->ClearColor(color);
	}
	void Renderer::Clear()
	{
		m_renderApi->Clear();
	}
	void Renderer::End()
	{
		m_renderApi->End();
	}
	void Renderer::Resize(unsigned int m_Width, unsigned int m_Height)
	{
		m_renderApi->Resize(m_Width, m_Height);
	}
	void Renderer::Submit(Ref<BufferGroup> group, MatrixFloat4x4 transform)
	{
		group->Bind();
		m_renderApi->DrawIndexed(group, 0);
	}
}