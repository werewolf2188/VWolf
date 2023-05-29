#include "vwpch.h"

#include "Graphics.h"

#include "RenderItem.h"

namespace VWolf {
/// Graphics

    Ref<Graphics> Graphics::graphicsImpl = nullptr;

    void Graphics::ClearColor(Color color) {
        if (graphicsImpl) {
            graphicsImpl->ClearColorImpl(color);
        }
    }

    void Graphics::Clear() {
        if (graphicsImpl) {
            graphicsImpl->ClearImpl();
        }
    }

    void Graphics::DrawMesh(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera) {
        if (graphicsImpl) {
            graphicsImpl->DrawMeshImpl(mesh, position, rotation, material, camera);
        }
    }

    void Graphics::AddLight(Light& light) {
        if (graphicsImpl) {
            graphicsImpl->AddLightImpl(light);
        }
    }

    void Graphics::RenderMesh(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera) {
        if (graphicsImpl) {
            graphicsImpl->RenderMeshImpl(mesh, transform, material, camera);
        }
    }

    void Graphics::SetRenderTexture(Ref<RenderTexture> renderTexture) {
        if (graphicsImpl) {
            graphicsImpl->SetRenderTextureImpl(renderTexture);
        }
    }

    void Graphics::BeginFrame() {
        if (graphicsImpl) {
            graphicsImpl->BeginFrameImpl();
        }
    }

    void Graphics::EndFrame() {
        if (graphicsImpl) {
            graphicsImpl->EndFrameImpl();
        }
    }
}
