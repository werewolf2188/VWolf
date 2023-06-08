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
        Vector4Float& GetBackgroundColor() { return backgroundColor; }
        void SetBackgroundColor(Vector4Float backgroundColor) { this->backgroundColor = backgroundColor; }
        void SetSkyboxMaterial(Material& material) { this->materialSkybox = &material; }
        MeshData& GetSkyboxMeshData() { return skybox; }
        Material& GetSkyboxMaterial() { return *this->materialSkybox; }
        void SetCamera(Ref<Camera> camera) { this->camera = camera; }
        Ref<Camera> GetCamera() { return this->camera; }
        Type GetType() { return type; }
        void SetType(Type type) { this->type = type; }
    public:
        SceneBackground& operator=(SceneBackground& t);
    private:
        Vector4Float backgroundColor;
        Type type = Type::Color;
        MeshData skybox;
        Material* materialSkybox;
        Ref<Camera> camera;
    };

    class Scene {
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
        void AddExistingGameObject(Ref<GameObject> gameObject);
        void RemoveGameObject(std::string name);
    public:
        std::vector<Ref<GameObject>> GetGameObjects() const { return gameObjects; }
        std::string GetName() const { return name; }
        void SetName(std::string name) { this->name = name; }
        SceneBackground& GetSceneBackground() { return sceneBackGround; }
        void SetSceneBackground(SceneBackground sceneBackground) { this->sceneBackGround = sceneBackground; }
    private:
        std::string name;
        entt::registry m_registry;

        std::vector<Ref<GameObject>> gameObjects;

        SceneBackground sceneBackGround;
        MeshData emptyMeshData;
        friend class GameObject;
    };
}
