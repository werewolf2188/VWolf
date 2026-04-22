//
//  BaseComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "entt/entt.hpp"

#include "VWolf/Core/Utils/UUID.h"

namespace VWolf {

    class GameObject;

    template<typename T>
    class ComponentInspector;

    class Component {
    public:
        Component(std::string name);
        ~Component();
    public:
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) = 0;
    public:
        const VWolf::UUID GetID() const { return id; }
        VWolf::UUID& GetID() { return id; }
        void SetID(VWolf::UUID value) { id = value; }
        std::string GetName() const { return name; }
        virtual void OnInspector() {}

        void SetGameObject(GameObject* gameObject) { this->gameObject = gameObject; }
        GameObject* GetGameObject() { return gameObject; }
    private:
        VWolf::UUID id;
        std::string name;
        GameObject* gameObject;
    };

    template<typename T>
    class ComponentInspector {
    public:
        ComponentInspector() {}
        ~ComponentInspector() {}
    public:
        virtual void OnInspector(T* component) {
            
        }
    };
}

#define VWOLF_COMPONENT_INSPECTOR_DEFINE(class) \
public:\
    static void SetComponentInspector(ComponentInspector<class> *componentInspector); \
private:\
    static ComponentInspector<class> *componentInspector;

#define VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(class) \
    void class::SetComponentInspector(ComponentInspector<class> *componentInspector) {\
        class::componentInspector = componentInspector;\
    }\
    ComponentInspector<class> * class::componentInspector = nullptr;
