//
//  GameObject.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/12/23.
//

#include "vwpch.h"

#include "GameObject.h"

namespace VWolf {
    GameObject::GameObject(std::string name): name(name), scene(nullptr) { }

    GameObject::GameObject(std::string name, entt::entity handle, Scene* scene): name(name), handle(handle), scene(scene) { }

    GameObject::GameObject(GameObject& gameObject) {
        this->name = gameObject.name;
    }

    GameObject::~GameObject() { }

    TransformComponent& GameObject::GetTransform() {
        return GetComponent<TransformComponent>();
    }

    void GameObject::OnInspector() {
        for (auto component: currentComponents) {
            component->OnInspector();
        }
    }

    void GameObject::AttachToScene(Scene* scene) {
        if (this->handle == entt::null)
            this->handle = scene->m_registry.create();
        this->scene = scene;
    }

    void GameObject::CopyComponents(GameObject* otherGameObject) {
        for(auto component: otherGameObject->currentComponents) {
            Component* newComponent = component->Copy(handle, scene->m_previewRegistry);
            newComponent->SetGameObject(otherGameObject);
            currentComponents.push_back(newComponent);
        }
    }
}
