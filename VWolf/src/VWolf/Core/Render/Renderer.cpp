#include "vwpch.h"

#include "Renderer.h"

#include "RenderItem.h"

namespace VWolf {
	Scope<RenderAPI> TestRenderer::m_renderApi = nullptr;

	void TestRenderer::Begin(Camera& camera, Ref<Shader> shader)
	{		
		m_renderApi->Begin(camera, shader);
	}
	void TestRenderer::ClearColor(Color color)
	{
		m_renderApi->ClearColor(color);
	}
	void TestRenderer::Clear()
	{
		m_renderApi->Clear();
	}
	void TestRenderer::End()
	{
		m_renderApi->End();
	}
	void TestRenderer::Submit(Ref<BufferGroup> group, MatrixFloat4x4 transform)
	{
		group->Bind();
		m_renderApi->DrawIndexed(group, 0);
	}

    Scope<Renderer> Renderer::rendererImpl = nullptr;

    void Renderer::Begin(Ref<Camera> camera) {
        rendererImpl->m_camera = camera;
    }

    void Renderer::ClearColor(Color color) {
        rendererImpl->backgroundColor = color;
    }

    void Renderer::SetShader(const char* shaderName) {
        rendererImpl->shaderName = shaderName;
    }

    void Renderer::DrawMesh(MeshData meshData, MatrixFloat4x4 transform) {
        rendererImpl->items.push_back(CreateRef<RenderItem>(meshData, transform));
    }

    void Renderer::End() {
        rendererImpl->ProcessItems();
        rendererImpl->items.clear();
    }
}
