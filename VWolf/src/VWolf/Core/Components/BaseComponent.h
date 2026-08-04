//
//  BaseComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "entt/entt.hpp"

#include "VWolf/Core/Object.h"
#include <boost/describe.hpp>
#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {

    class GameObject;

    class Component: public Object {
    public:
        Component(std::string name);
        Component(std::string name, UUID id);
        virtual ~Component();
    public:
        virtual Ref<Component> Copy(entt::entity& handle, entt::registry& registry) = 0;
    protected:
        template<typename T>
        Ref<T> CopyComponent(entt::entity& handle, entt::registry& registry) {
            T& component = registry.emplace<T>(handle, (T&)*this);
            // Non-owning: entt owns the component storage.
            return UnownedRef<T>(&component);
        }
    public:
        void SetGameObject(Weak<GameObject> gameObject);
        Ref<GameObject> GetGameObject() {
            if (Ref<GameObject> go = gameObject.lock()) {
                return go;
            }
            return nullptr;
        }
    private:
        Weak<GameObject> gameObject;
        
        BOOST_DESCRIBE_CLASS(Component, (Object), (), (id), ())
    };
}
