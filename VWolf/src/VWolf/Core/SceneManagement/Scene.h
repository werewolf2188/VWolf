//
//  Scene.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/11/23.
//

#pragma once

#include "entt/entt.hpp"

#include "VWolf/Core/Render/Camera.h"
#include "VWolf/Core/Render/Material.h"
#include "VWolf/Core/Render/Mesh.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

#include "VWolf/Core/Components/CameraComponent.h"

namespace reactphysics3d {
    class PhysicsWorld;
}

namespace VWolf {
    class GameObject;

    class SceneBackground {
    public:
        enum class Type: unsigned int {
            Color, Skybox
        };
        SceneBackground();
        SceneBackground(const SceneBackground& scene);
        SceneBackground(SceneBackground&& scene) = default;
        ~SceneBackground();
    public:
        Color& GetBackgroundColor() { return backgroundColor; }
        void SetBackgroundColor(Color backgroundColor) { this->backgroundColor = backgroundColor; }
        void SetSkyboxMaterial(Ref<Material> material) { this->materialSkybox = material; }
        Ref<Mesh> GetSkyboxMesh() { return skyboxEx; }
        Material& GetSkyboxMaterial() { return *this->materialSkybox; }
        Ref<Material> GetSkyboxMaterialEx() { return this->materialSkybox; }
        Type GetType() { return type; }
        void SetType(Type type) { this->type = type; }
    public:
        SceneBackground& operator=(const SceneBackground& t);
        SceneBackground& operator=(SceneBackground&& t);
    private:
        void Draw(Ref<CameraComponent> camera);
    private:
        Color backgroundColor;
        Type type = Type::Color;
        Ref<Mesh> skyboxEx;
        Ref<Material> materialSkybox;
        
        friend class Scene;
        
        BOOST_DESCRIBE_CLASS(SceneBackground, (), (), (), (backgroundColor, type))
        VWOLF_SERIALIZATION_FRIENDS(SceneBackground)
    };

    BOOST_DESCRIBE_ENUM(SceneBackground::Type, Color, Skybox)

    class Scene: public Object, public Shareable<Scene> {
    public:
        Scene(std::string name);
        Scene(): Object(UUID::NewUUID()) {};
        Scene(const Scene& scene);
        ~Scene();
    public:
        Ref<GameObject> CreateGameObject(std::string name);
        void UpdateEditor();
        void DrawEditor();
        void DrawPreviewEditor();
        void AddExistingGameObject(Ref<GameObject> gameObject);
        void RemoveGameObject(std::string name);
        void StartingPreview();
        void StopingPreview();
        void Save(std::filesystem::path path);
    public:
        std::vector<Ref<GameObject>> GetGameObjects() const { return isPreviewing ? previewGameObjects : gameObjects; }
        void SetName(std::string name) { this->name = name; }
        SceneBackground& GetSceneBackground() { return sceneBackGround; }
        void SetSceneBackground(SceneBackground& sceneBackground) { this->sceneBackGround = sceneBackground; }
        entt::registry& CurrentRegistry() { return isPreviewing ? m_previewRegistry : m_registry; }
    public:
        Scene& operator=(const Scene& t);
    public:
        static Ref<Scene> Load(std::filesystem::path path, UUID _id);
    private:
        void Draw(Ref<CameraComponent> camera);
        void FindNextCamera();
    private:
        bool isPreviewing = false;
        float previewAccumulator = 0.2f;
        entt::registry m_registry;
        entt::registry m_previewRegistry;
        SceneBackground sceneBackGround;
        
        std::vector<Ref<GameObject>> gameObjects, previewGameObjects;

        Ref<Mesh> emptyMesh;
        reactphysics3d::PhysicsWorld *world;

        friend class GameObject;
        
        BOOST_DESCRIBE_CLASS(Scene, (Object), (), (name), (sceneBackGround))
        VWOLF_SERIALIZATION_FRIENDS(Scene)
    };
}

namespace YAML {
    bool DeserializeGameObjects(const Node& node, VWolf::Scene& rhs);

    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::SceneBackground::Type, Color, Skybox)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::SceneBackground)

    template<>
    struct convert<VWolf::Scene>
    {
        static bool decode(const Node& node, VWolf::Scene& rhs)
        {
            return VWolf::DeserializeFromBoostDescribe(node, rhs);
        }
    };
}
