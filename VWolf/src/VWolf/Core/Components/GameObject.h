//
//  GameObject.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/12/23.
//

#pragma once

#include "entt/entt.hpp"

#include "Components.h"

#include "VWolf/Core/Object.h"
#include "VWolf/Core/SceneManagement/Scene.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

namespace reactphysics3d{
    class RigidBody;
}

namespace VWolf {
    class GameObject;
}

namespace YAML {
    bool DeserializeComponents(const Node& node, VWolf::GameObject& rhs);
}

namespace VWolf {

    class GameObject: public Object {
    public:
        GameObject(): Object(UUID::NewUUID()) {};
        GameObject(std::string name);
        GameObject(std::string name, entt::entity handle, Scene* scene);
        GameObject(const GameObject& gameObject);
        GameObject(GameObject&& gameObject);
        ~GameObject();
    public:
        TransformComponent& GetTransform();
    public:
        void SetName(const std::string& name) {
            this->name = name;
        }
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
        T& GetComponent() const
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
        bool HasComponent() const
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
        void AttachToScene(Scene* scene);
        void CopyComponents(GameObject* otherGameObject);
    public:
        reactphysics3d::RigidBody* GetRigidBody() { return mRigidBody; }
        void SetRigidBody(reactphysics3d::RigidBody* rigidBody) { mRigidBody = rigidBody; }
        std::vector<Component*>& GetCurrentComponents() { return currentComponents; }
    private:
        entt::entity handle { entt::null };
        Scene* scene;

        std::vector<Component*> currentComponents;

        reactphysics3d::RigidBody* mRigidBody = nullptr;
        
        BOOST_DESCRIBE_CLASS(GameObject, (Object), (), (id, name), ())
        VWOLF_SERIALIZATION_FRIENDS(GameObject)
        
        friend bool YAML::DeserializeComponents(const YAML::Node& node, VWolf::GameObject& rhs);
    };
}

namespace YAML {
    template<>
    struct convert<VWolf::GameObject> {
        static bool decode(const Node& node, VWolf::GameObject& rhs)
        {
            bool boostDeserialization = DeserializeFromBoostDescribe(node, rhs);
            bool componentsDeserialization = DeserializeComponents(node, rhs);
            
            return boostDeserialization && componentsDeserialization;
        }
    };
}
