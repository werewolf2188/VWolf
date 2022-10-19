#include "vwpch.h"

#include "Renderer.h"

#include "RenderItem.h"

namespace VWolf {
    Scope<Renderer> Renderer::rendererImpl = nullptr;

    void Renderer::Begin(Ref<Camera> camera) {
        rendererImpl->m_camera = camera;
    }

    void Renderer::ClearColor(Color color) {
        rendererImpl->backgroundColor = color;
        rendererImpl->clearColor = true;
    }

    void Renderer::Clear() {
        rendererImpl->clearDepthStencil = true;
    }

    void Renderer::SetShader(const char* shaderName) {
        rendererImpl->shaderName = shaderName;
    }

    void Renderer::DrawMesh(MeshData& meshData, MatrixFloat4x4 transform) {
        rendererImpl->items.push_back(CreateRef<RenderItem>(meshData, rendererImpl->shaderName, transform));
    }

    void Renderer::End() {
        rendererImpl->ProcessItems();
        rendererImpl->items.clear();
    }
}
