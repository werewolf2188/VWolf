//
//  Scene.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/11/23.
//

#pragma once

#include "entt/entt.hpp"

#include "VWolf/Core/Render/Camera.h"

#include "VWolf/Core/Math/VMath.h"

namespace VWolf {
    class GameObject;

    class SceneBackground {
    public:
        SceneBackground();
        SceneBackground(SceneBackground& scene);
        SceneBackground(SceneBackground&& scene) = default;
        ~SceneBackground();
    public:
        Vector4Float& GetBackgroundColor() { return backgroundColor; }
        void SetBackgroundColor(Vector4Float backgroundColor) { this->backgroundColor = backgroundColor; }
    public:
        SceneBackground& operator=(SceneBackground& t);
    private:
        Vector4Float backgroundColor;
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
