#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Components/CameraComponent.h"
#include "Shader.h"
#include "Material.h"
#include "Light.h"
#include "Mesh.h"

#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Time.h"

namespace VWolf {
    class Graphics {
    public:
        virtual ~Graphics() = default;
    public:
        static void SetRenderTexture(Ref<RenderTexture> renderTexture);
        
    public:
        static void DrawMesh(Ref<Mesh> mesh, Vector3 position, Quaternion rotation, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> camera = nullptr, bool castShadows = true, bool receiveShadows = true);
        static void DrawMesh(Ref<Mesh> mesh, Matrix4x4 transform, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> camera = nullptr, bool castShadows = true, bool receiveShadows = true);
    };
}
