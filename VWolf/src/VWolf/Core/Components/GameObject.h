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

    class GameObject: public Object, public Shareable<GameObject> {
    public:
        GameObject(): Object(UUID::NewUUID()) {};
        GameObject(std::string name);
        GameObject(std::string name, entt::entity handle, Weak<Scene> scene);
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
            T& component = GetScene()->CurrentRegistry().emplace<T>(handle, std::forward<Args>(args)...);
            component.SetGameObject(this->weak_from_this());
            SaveComponent(UnownedRef<T>(&component));
            return component;
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)
        {
            T& component = GetScene()->CurrentRegistry().emplace_or_replace<T>(handle, std::forward<Args>(args)...);
            // TODO: Find and replace
            SaveComponent(UnownedRef<T>(&component));
            return component;
        }

        template<typename T>
        T& GetComponent()
        {
            VWOLF_CLIENT_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return GetScene()->CurrentRegistry().get<T>(handle);
        }
        
        template<typename T>
        T& GetComponent() const
        {
            VWOLF_CLIENT_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return GetScene()->CurrentRegistry().get<T>(handle);
        }

        template<typename T>
        bool HasComponent()
        {
            return GetScene()->CurrentRegistry().try_get<T>(handle);
        }
        
        template<typename T>
        bool HasComponent() const
        {
            return GetScene()->CurrentRegistry().try_get<T>(handle);
        }

        template<typename T>
        void RemoveComponent()
        {
            VWOLF_CLIENT_ASSERT(HasComponent<T>(), "Entity does not have component!");
            auto name = GetComponent<T>().GetName();
            GetScene()->CurrentRegistry().remove<T>(handle);
            int i = 0;
            bool found = false;
            for(; i < currentComponents.size(); i++) {
                if (auto component = currentComponents[i]) {
                    if (component->GetName() == name) {
                        found = true;
                        break;
                    }
                }
            }
            if (found) {
                currentComponents.erase(currentComponents.begin() + i);
            }
        }
    public:
        void AttachToScene(Weak<Scene> scene);
        void CopyComponents(Ref<GameObject> otherGameObject);
        void ClearCurrentComponents();
    public:
        reactphysics3d::RigidBody* GetRigidBody() { return mRigidBody; }
        void SetRigidBody(reactphysics3d::RigidBody* rigidBody) { mRigidBody = rigidBody; }
        const std::vector<Ref<Component>>& GetCurrentComponents() const { return currentComponents; }
        Ref<Scene> GetScene() const {
            if (Ref<Scene> sc = scene.lock()) {
                return sc;
            }
            return nullptr;
        }
    private:
        void SaveComponent(const Ref<Component>& component);
    private:
        entt::entity handle { entt::null };
        Weak<Scene> scene;

        std::vector<Ref<Component>> currentComponents;

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
            return DeserializeFromBoostDescribe(node, rhs);
        }
        
        static bool decodeComponents(const Node& node, VWolf::GameObject& rhs)
        {
            return DeserializeComponents(node, rhs);
        }
    };
}
