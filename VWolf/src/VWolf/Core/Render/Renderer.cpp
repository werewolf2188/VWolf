#include "vwpch.h"

#include "Renderer.h"

#include "RenderItem.h"

#include "VWolf/Core/Time.h"

namespace VWolf {
    Scope<Renderer> Renderer::rendererImpl = nullptr;

    void Renderer::Begin(Ref<PerspectiveCamera> camera) {
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

    CameraPass Renderer::GetCameraPass() {
        return {
            m_camera->GetViewMatrix(),
            inverse(m_camera->GetViewMatrix()),
            m_camera->GetProjection(),
            inverse(m_camera->GetProjection()),
            m_camera->GetViewProjection(),
            inverse(m_camera->GetViewProjection()),
            m_camera->GetPosition(),
            m_camera->GetDisplaySize(),
            { 1 / m_camera->GetDisplaySize().x, 1 / m_camera->GetDisplaySize().y },
            m_camera->GetNearZ(),
            m_camera->GetFarZ(),
            Time::GetTotalTime(),
            Time::GetDeltaTime()
        };
    }
}
