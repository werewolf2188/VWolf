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

    GameObject::~GameObject() { }

    TransformComponent& GameObject::GetTransform() {
        return GetComponent<TransformComponent>();
    }

    void GameObject::OnInspector() {
        for (auto component: currentComponents) {
            component->OnInspector();
        }
    }
}
