#include "vwpch.h"

#include "Graphics.h"

#include "InternalGraphics.h"
#include "GraphicsContext.h"

namespace VWolf {
/// Graphics

    void Graphics::SetRenderTexture(Ref<RenderTexture> renderTexture) {
        if (InternalGraphics::Singleton())
            InternalGraphics::Singleton()->SetRenderTexture(renderTexture);
    }

    void Graphics::DrawMesh(Ref<Mesh> mesh, Vector3 position, Quaternion rotation, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> camera, bool castShadows, bool receiveShadows) {
        GraphicsContext::GetList().Add(CreateRef<DrawMeshCommand>(mesh, position, rotation, material, layer, submeshIndex, camera, castShadows, receiveShadows));
    }

    void Graphics::DrawMesh(Ref<Mesh> mesh, Matrix4x4 transform, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> camera, bool castShadows, bool receiveShadows) {
        GraphicsContext::GetList().Add(CreateRef<DrawMeshCommand>(mesh, transform, material, layer, submeshIndex, camera, castShadows, receiveShadows));
    }
}
