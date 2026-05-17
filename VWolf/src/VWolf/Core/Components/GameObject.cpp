//
//  GameObject.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/12/23.
//

#include "vwpch.h"

#include "GameObject.h"
#include "Components.h"

#define DESERIALIZE_COMPONENT(T) \
if (nodeComponent[#T]) { \
    VWolf::T temp  = nodeComponent[#T].as<VWolf::T>(); \
    rhs.AddComponent<VWolf::T>(temp); \
}

#define SERIALIZE_COMPONENT(T) \
if (v.HasComponent<T>()) {\
    out << v.GetComponent<T>();\
}

const std::string componentKey = "Components";

namespace YAML {

    bool DeserializeComponents(const Node& node, VWolf::GameObject& rhs) {
        rhs.AttachToScene(VWolf::Scene::currentScene);
        if (node[componentKey]) {
            for (auto& nodeComponent: node[componentKey]) {
                DESERIALIZE_COMPONENT(TransformComponent)
                DESERIALIZE_COMPONENT(ShapeRendererComponent)
                DESERIALIZE_COMPONENT(MeshFilterComponent)
                DESERIALIZE_COMPONENT(MeshRendererComponent)
                DESERIALIZE_COMPONENT(LightComponent)
                DESERIALIZE_COMPONENT(CameraComponent)
                DESERIALIZE_COMPONENT(RigidBodyComponent)
                DESERIALIZE_COMPONENT(MeshColliderComponent)
                DESERIALIZE_COMPONENT(SphereColliderComponent)
                DESERIALIZE_COMPONENT(BoxColliderComponent)
                DESERIALIZE_COMPONENT(AudioListenerComponent)
                DESERIALIZE_COMPONENT(AudioSourceComponent)
            }
        }
        return true;
    }
}

namespace VWolf {
    
    YAML::Emitter& SerializeComponents(YAML::Emitter& out, GameObject& v) {
        out << YAML::Key << componentKey;
        out << YAML::BeginSeq;

        SERIALIZE_COMPONENT(TransformComponent)
        SERIALIZE_COMPONENT(ShapeRendererComponent)
        SERIALIZE_COMPONENT(MeshFilterComponent)
        SERIALIZE_COMPONENT(MeshRendererComponent)
        SERIALIZE_COMPONENT(LightComponent)
        SERIALIZE_COMPONENT(CameraComponent)
        SERIALIZE_COMPONENT(RigidBodyComponent)
        SERIALIZE_COMPONENT(MeshColliderComponent)
        SERIALIZE_COMPONENT(SphereColliderComponent)
        SERIALIZE_COMPONENT(BoxColliderComponent)
        SERIALIZE_COMPONENT(AudioListenerComponent)
        SERIALIZE_COMPONENT(AudioSourceComponent)

        out << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& SerializeComponents(YAML::Emitter& out, const GameObject& v) {
        out << YAML::Key << componentKey;
        out << YAML::BeginSeq;

        SERIALIZE_COMPONENT(TransformComponent)
        SERIALIZE_COMPONENT(ShapeRendererComponent)
        SERIALIZE_COMPONENT(MeshFilterComponent)
        SERIALIZE_COMPONENT(MeshRendererComponent)
        SERIALIZE_COMPONENT(LightComponent)
        SERIALIZE_COMPONENT(CameraComponent)
        SERIALIZE_COMPONENT(RigidBodyComponent)
        SERIALIZE_COMPONENT(MeshColliderComponent)
        SERIALIZE_COMPONENT(SphereColliderComponent)
        SERIALIZE_COMPONENT(BoxColliderComponent)
        SERIALIZE_COMPONENT(AudioListenerComponent)
        SERIALIZE_COMPONENT(AudioSourceComponent)

        out << YAML::EndSeq;
        return out;
    }

    GameObject::GameObject(std::string name): name(name), scene(nullptr) { }

    GameObject::GameObject(std::string name, entt::entity handle, Scene* scene): name(name), handle(handle), scene(scene) { }

    GameObject::GameObject(GameObject& gameObject) {
        this->id = gameObject.id;
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

    YAML::Emitter& operator<<(YAML::Emitter& out, GameObject& v) {
        out << YAML::BeginMap;
        SerializeFromBoostOnlyMembers(out, v);
        SerializeComponents(out, v);
        out << YAML::EndMap;
        
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const GameObject& v) {
        out << YAML::BeginMap;
        SerializeFromBoostOnlyMembers(out, v);
        SerializeComponents(out, v);
        out << YAML::EndMap;
        
        return out;
    }
}
