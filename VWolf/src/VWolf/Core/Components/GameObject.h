//
//  GameObject.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/12/23.
//

#pragma once

#include "entt/entt.hpp"

#include "Components.h"

#include "VWolf/Core/SceneManagement/Scene.h"

namespace VWolf {

    class GameObject {
    public:
        GameObject(std::string name);
        GameObject(std::string name, entt::entity handle, Scene* scene);
        ~GameObject();
    public:
        TransformComponent& GetTransform();
    public:
        std::string GetName() const { return name; }
    public:
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            VWOLF_CLIENT_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = scene->m_registry.emplace<T>(handle, std::forward<Args>(args)...);
            component.SetGameObject(this);
            currentComponents.push_back(&component);
            return component;
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)
        {
            T& component = scene->m_registry.emplace_or_replace<T>(handle, std::forward<Args>(args)...);
            // TODO: Find and replace
            currentComponents.emplace_back(&component);
            return component;
        }

        template<typename T>
        T& GetComponent()
        {
            VWOLF_CLIENT_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return scene->m_registry.get<T>(handle);
        }

        template<typename T>
        bool HasComponent()
        {
            return scene->m_registry.try_get<T>(handle);
        }

        template<typename T>
        void RemoveComponent()
        {
            VWOLF_CLIENT_ASSERT(HasComponent<T>(), "Entity does not have component!");
            scene->m_registry.remove<T>(handle);
        }

        void OnInspector();
    private:
        entt::entity handle { entt::null };
        Scene* scene;
        std::string name;

        std::vector<Component*> currentComponents;
    };
}
