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

namespace reactphysics3d{
    class RigidBody;
}

namespace VWolf {

    class GameObject {
    public:
        GameObject() = default;
        GameObject(std::string name);
        GameObject(std::string name, entt::entity handle, Scene* scene);
        GameObject(GameObject& gameObject);
        GameObject(GameObject&& gameObject) = default;
        ~GameObject();
    public:
        TransformComponent& GetTransform();
    public:
        std::string GetName() const { return name; }
        void SetName(std::string name) { this->name = name; }
        entt::entity GetHandle() { return handle; }
    public:
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            VWOLF_CLIENT_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = scene->CurrentRegistry().emplace<T>(handle, std::forward<Args>(args)...);
            component.SetGameObject(this);
            currentComponents.push_back(&component);
            return component;
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)
        {
            T& component = scene->CurrentRegistry().emplace_or_replace<T>(handle, std::forward<Args>(args)...);
            // TODO: Find and replace
            currentComponents.emplace_back(&component);
            return component;
        }

        template<typename T>
        T& GetComponent()
        {
            VWOLF_CLIENT_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return scene->CurrentRegistry().get<T>(handle);
        }

        template<typename T>
        bool HasComponent()
        {
            return scene->CurrentRegistry().try_get<T>(handle);
        }

        template<typename T>
        void RemoveComponent()
        {
            VWOLF_CLIENT_ASSERT(HasComponent<T>(), "Entity does not have component!");
            auto name = GetComponent<T>().GetName();
            scene->CurrentRegistry().remove<T>(handle);
            // Need to remove it from currentComponents
            int i = 0;
            bool found = false;
            for(; i < currentComponents.size(); i++) {
                if (currentComponents[i]->GetName() == name) {
                    found = true;
                    break;
                }
            }
            if (found)
                currentComponents.erase(currentComponents.begin() + i);
        }
    public:
        void OnInspector();
        void AttachToScene(Scene* scene);
        void CopyComponents(GameObject* otherGameObject);
    public:
        reactphysics3d::RigidBody* GetRigidBody() { return mRigidBody; }
        void SetRigidBody(reactphysics3d::RigidBody* rigidBody) { mRigidBody = rigidBody; }
    private:
        entt::entity handle { entt::null };
        Scene* scene;
        std::string name;

        std::vector<Component*> currentComponents;

        reactphysics3d::RigidBody* mRigidBody = nullptr;
    };
}
