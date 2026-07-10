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
        ~Component();
    public:
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) = 0;
    public:
        void SetGameObject(GameObject* gameObject);
        GameObject* GetGameObject() { return gameObject; }
    private:
        GameObject* gameObject;
        
        BOOST_DESCRIBE_CLASS(Component, (Object), (), (id), ())
    };
}
