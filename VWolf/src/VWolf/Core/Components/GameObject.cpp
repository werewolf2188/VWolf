//
//  GameObject.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/12/23.
//

#include "vwpch.h"

#include "GameObject.h"
#include "Components.h"

#include <boost/type_index.hpp>

template <typename... Ts>
struct TypeList {
    template <typename F>
    static void for_each(F&& f) {
        // Expand the pack Ts... and call f for each type T
        (f.template operator()<Ts>(), ...);
    }
};

using AllComponents = TypeList<
                        VWolf::TransformComponent,
                        VWolf::ShapeRendererComponent,
                        VWolf::MeshFilterComponent,
                        VWolf::MeshRendererComponent,
                        VWolf::LightComponent,
                        VWolf::CameraComponent,
                        VWolf::RigidBodyComponent,
                        VWolf::MeshColliderComponent,
                        VWolf::SphereColliderComponent,
                        VWolf::BoxColliderComponent,
                        VWolf::AudioListenerComponent,
                        VWolf::AudioSourceComponent
                    >;

const std::string componentKey = "Components";

namespace YAML {

    template<typename type>
    bool DeserializeComponent(const Node& node, VWolf::GameObject& rhs) {

        std::string typeName = boost::typeindex::type_id_with_cvr<type>().pretty_name();
        std::string toRemove = "VWolf::";
        size_t pos = typeName.find(toRemove);
        if (pos != std::string::npos) {
            typeName.erase(pos, toRemove.length());
        }
        if (node[typeName]) {
            type temp  = node[typeName].as<type>();
            rhs.AddComponent<type>(temp);
        }
        return true;
    }

    bool DeserializeComponents(const Node& node, VWolf::GameObject& rhs) {
        rhs.AttachToScene(VWolf::Scene::currentScene);
        if (node[componentKey]) {
            for (auto& nodeComponent: node[componentKey]) {
                
                AllComponents::for_each([&]<typename T>() {
                    DeserializeComponent<T>(nodeComponent, rhs);
                });
            }
        }
        return true;
    }
}

namespace VWolf {
    
    template<typename T>
    YAML::Emitter& SerializeComponent(YAML::Emitter& out, GameObject& v) {
        if (v.HasComponent<T>()) {
            out << v.GetComponent<T>();
        }
        return out;
    }

    template<typename T>
    YAML::Emitter& SerializeComponent(YAML::Emitter& out, const GameObject& v) {
        if (v.HasComponent<T>()) {
            out << v.GetComponent<T>();
        }
        return out;
    }

    YAML::Emitter& SerializeComponents(YAML::Emitter& out, GameObject& v) {
        out << YAML::Key << componentKey;
        out << YAML::BeginSeq;

        AllComponents::for_each([&]<typename T>() {
            SerializeComponent<T>(out, v);
        });

        out << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& SerializeComponents(YAML::Emitter& out, const GameObject& v) {
        out << YAML::Key << componentKey;
        out << YAML::BeginSeq;

        AllComponents::for_each([&]<typename T>() {
            SerializeComponent<T>(out, v);
        });

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
