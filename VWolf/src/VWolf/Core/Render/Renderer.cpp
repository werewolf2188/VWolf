#include "vwpch.h"

#include "Renderer.h"

#include "RenderItem.h"

#include "VWolf/Core/Time.h"

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

    void Renderer::SetMaterial(AbstractMaterial& material) {
        rendererImpl->material = &material;
    }

//    void Renderer::SetLight(Light& light) {
//        rendererImpl->light = &light;
//    }

    void Renderer::AddLight(Light& light) {
        rendererImpl->lights.push_back(light);
    }

    void Renderer::DrawMesh(MeshData& meshData, MatrixFloat4x4 transform) {
        rendererImpl->items.push_back(CreateRef<RenderItem>(meshData, *rendererImpl->material, transform));
    }

    void Renderer::End() {
        rendererImpl->ProcessItems();
        rendererImpl->items.clear();
//        rendererImpl->light = nullptr;
        rendererImpl->lights.clear();
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
