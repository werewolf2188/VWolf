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
        ~SceneBackground();
    public:
        Vector4Float& GetBackgroundColor() { return backgroundColor; }
    private:
        Vector4Float backgroundColor;
    };

    class Scene {
    public:
        Scene(std::string name);
        ~Scene();
    public:
        Ref<GameObject> CreateGameObject(std::string name);
        void UpdateEditor();
        void DrawEditor(Ref<Camera> editorCamera);
    public:
        std::vector<Ref<GameObject>> GetGameObjects() const { return gameObjects; }
        std::string GetName() const { return name; }
        SceneBackground& GetSceneBackground() { return sceneBackGround; }
    private:
        std::string name;
        entt::registry m_registry;

        std::vector<Ref<GameObject>> gameObjects;

        SceneBackground sceneBackGround;
        friend class GameObject;
    };
}
