//
//  GraphicsContext.h
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Color.h"
#include "VWolf/Core/Math/VMath.h"
#include "GraphicsCommands.h"
#include "VWolf/Core/Components/Components.h"

namespace VWolf {

    struct GraphicsCommandEntity {
        UUID id;
        Ref<Mesh> mesh;
        Matrix4x4 transform;
        uint32_t subMeshIndex;
        bool castShadows;
        
        GraphicsCommandEntity(Ref<Mesh> mesh, Matrix4x4 transform, uint32_t subMeshIndex, bool castShadows):
        mesh(mesh), transform(transform), subMeshIndex(subMeshIndex), castShadows(castShadows) {}
    };

    // TODO: Move
    struct FreeDeleter {
        void operator()(void* ptr) const {
            std::free(ptr);
        }
    };

    using UniqueVoidPtr = std::unique_ptr<void, FreeDeleter>;
    //

    struct CommandsContext {
        Bounds sceneBounds;
        std::unordered_set<Ref<CameraComponent>, RefHash<CameraComponent>> cameras;
        std::unordered_set<Ref<LightComponent>, RefHash<LightComponent>> lights;
        std::unordered_set<Ref<Material>, RefHash<Material>> materials;
        std::unordered_set<Ref<Shader>, RefHash<Shader>> shaders;
        
        std::vector<GraphicsCommandEntity> entities;
        
        std::map<UUID, std::set<UUID>> shaderMaterials;
        std::multimap<UUID, UUID> materialObjects;
        std::multimap<UUID, UUID> objectLights;
        
        std::map<std::tuple<UUID, UUID>, Matrix4x4> lightSpaceMatrix;
        std::map<UUID, UniqueVoidPtr> materialData;
        std::map<UUID, CameraPass> gpuCamera;
        std::map<UUID, CameraPass> gpuSkyboxCamera;
        
        Ref<Shader> shadowShader;
        
        bool EntityContainsLight(const GraphicsCommandEntity& entity, UUID light);
    };

    // This class will help to separate the logic to add the information that's not related to the commands but it needs to be sent to the GPU
    // Such as the lights, the clear color, etc... Mostly repeated code among the internal graphics classes.
    // There's a lot of repeated code that should not be inside those classes but be preprocessed before getting to the draw calls.
    class GraphicsContext {
    public:
        static void Reset();
        static void AddLight(LightComponent& light);
    public:
        static GraphicsCommandList& GetList() { return list; }
        static Color GetBackgroundColor() { return backgroundColor; }
        static void SetClearColor(Color _backgroundColor) { backgroundColor = _backgroundColor; }
        static std::vector<LightComponent>& GetLights_Ex() { return lights_ex; }
        static std::vector<Light>& GetLights() { return lights; }
        static std::vector<Matrix4x4>& GetLightsSpaces() { return lightSpaces; }
        
        // TODO: NEW
        static Ref<CommandsContext> ProcessContext();
    private:
        
        static std::vector<LightComponent> GetLightsFor(const Matrix4x4& transform);
        
        static GraphicsCommandList list;
        static std::vector<LightComponent> lights_ex;
        static std::vector<Light> lights;
        static std::vector<Matrix4x4> lightSpaces;
        static Color backgroundColor;
    };
}
