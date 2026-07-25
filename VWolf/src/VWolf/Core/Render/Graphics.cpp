#include "vwpch.h"

#include "Graphics.h"

#include "InternalGraphics.h"
#include "GraphicsContext.h"

namespace VWolf {
/// Graphics

    void Graphics::DrawMesh(Ref<Mesh> mesh1, Vector4 position, Vector4 rotation, Material& material, Ref<Camera> camera) {
        if (InternalGraphics::GetGraphicsImpl())
            InternalGraphics::GetGraphicsImpl()->DrawMeshImpl(mesh1, position, rotation, material, camera);
    }

    void Graphics::RenderMesh(Ref<Mesh> mesh1, Matrix4x4 transform, Material& material, Ref<Camera> camera) {
        if (InternalGraphics::GetGraphicsImpl())
            InternalGraphics::GetGraphicsImpl()->RenderMeshImpl(mesh1, transform, material, camera);
    }

    void Graphics::SetRenderTexture(Ref<RenderTexture> renderTexture) {
        if (InternalGraphics::GetGraphicsImpl())
            InternalGraphics::GetGraphicsImpl()->SetRenderTextureImpl(renderTexture);
    }

    void Graphics::DrawMesh(Ref<Mesh> mesh, Vector3 position, Quaternion rotation, Ref<Material> material, int layer, int submeshIndex, Ref<Camera> camera, bool castShadows, bool receiveShadows) {
        GraphicsContext::GetList().Add(CreateRef<DrawMeshCommand>(mesh, position, rotation, material, layer, submeshIndex, camera, castShadows, receiveShadows));
    }

    void Graphics::DrawMesh(Ref<Mesh> mesh, Matrix4x4 transform, Ref<Material> material, int layer, int submeshIndex, Ref<Camera> camera, bool castShadows, bool receiveShadows) {
        GraphicsContext::GetList().Add(CreateRef<DrawMeshCommand>(mesh, transform, material, layer, submeshIndex, camera, castShadows, receiveShadows));
    }
}
