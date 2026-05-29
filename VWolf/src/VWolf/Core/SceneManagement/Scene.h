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
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

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
        SceneBackground(SceneBackground& scene);
        SceneBackground(SceneBackground&& scene) = default;
        ~SceneBackground();
    public:
        Color& GetBackgroundColor() { return backgroundColor; }
        void SetBackgroundColor(Color backgroundColor) { this->backgroundColor = backgroundColor; }
        void SetSkyboxMaterial(Material& material) { this->materialSkybox = &material; }
        MeshData& GetSkyboxMeshData() { return skybox; }
        Material& GetSkyboxMaterial() { return *this->materialSkybox; }
        void SetCamera(Ref<Camera> camera) { this->camera = camera; }
        Ref<Camera> GetCamera() { return this->camera; }
        Type GetType() { return type; }
        void SetType(Type type) { this->type = type; }
    public:
        SceneBackground& operator=(SceneBackground& t);
        SceneBackground& operator=(SceneBackground&& t);
    private:
        Color backgroundColor;
        Type type = Type::Color;
        MeshData skybox;
        Material* materialSkybox;
        Ref<Camera> camera;
        
        BOOST_DESCRIBE_CLASS(SceneBackground, (), (), (), (backgroundColor, type))
        VWOLF_SERIALIZATION_FRIENDS(SceneBackground)
    };

    BOOST_DESCRIBE_ENUM(SceneBackground::Type, Color, Skybox)

    class Scene: public IIdentifiable {
    public:
        Scene(std::string name);
        Scene() = default;
        Scene(Scene& scene);
        Scene(Scene&& scene);
        ~Scene();
    public:
        Ref<GameObject> CreateGameObject(std::string name);
        void UpdateEditor();
        void DrawEditor(Ref<Camera> editorCamera);
        void DrawPreviewEditor();
        void AddExistingGameObject(Ref<GameObject> gameObject);
        void RemoveGameObject(std::string name);
        void StartingPreview();
        void StopingPreview();
        void Save(std::filesystem::path path);
    public:
        std::vector<Ref<GameObject>> GetGameObjects() const { return isPreviewing ? previewGameObjects : gameObjects; }
        std::string GetName() const { return name; }
        void SetName(std::string name) { this->name = name; }
        SceneBackground& GetSceneBackground() { return sceneBackGround; }
        void SetSceneBackground(SceneBackground& sceneBackground) { this->sceneBackGround = sceneBackground; }
        entt::registry& CurrentRegistry() { return isPreviewing ? m_previewRegistry : m_registry; }
    public:
        static Ref<Scene> Load(std::filesystem::path path);
    public:
        static Scene* currentScene;
    private:
        bool isPreviewing = false;
        std::string name;
        entt::registry m_registry, m_previewRegistry;

        std::vector<Ref<GameObject>> gameObjects, previewGameObjects;

        SceneBackground sceneBackGround;
        MeshData emptyMeshData, testData;
        reactphysics3d::PhysicsWorld *world;

        float previewAccumulator = 0.2f;
        friend class GameObject;
        
        BOOST_DESCRIBE_CLASS(Scene, (IIdentifiable), (), (id), (name, sceneBackGround))
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
            VWolf::Scene::currentScene = &rhs;

            VWolf::DeserializeFromBoostDescribe(node, rhs);
            
            DeserializeGameObjects(node, rhs);

            VWolf::Scene::currentScene = nullptr;
            return true;
        }
    };
}
