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
#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>

using AllComponents = boost::mpl::list<
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
    struct ComponentDeserializer {
    public:
        ComponentDeserializer(const Node& node, VWolf::GameObject& rhs):
        node(node), rhs(rhs) {}
        
        template <typename type>
        void operator()(type) const {
            std::string typeName = boost::typeindex::type_id_with_cvr<type>().pretty_name();

#ifdef VWOLF_PLATFORM_WINDOWS
            std::string toRemove = "class VWolf::";
#elif defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            std::string toRemove = "VWolf::";
#endif

            size_t pos = typeName.find(toRemove);
            if (pos != std::string::npos) {
                typeName.erase(pos, toRemove.length());
            }
            if (node[typeName]) {
                type temp  = node[typeName].as<type>();
                rhs.AddComponent<type>(temp);
            }
        }
    private:
        const Node& node;
        VWolf::GameObject& rhs;
    };

    bool DeserializeComponents(const Node& node, VWolf::GameObject& rhs) {
        rhs.AttachToScene(VWolf::Scene::currentScene);
        if (node[componentKey]) {
            for (auto& nodeComponent: node[componentKey]) {
                boost::mpl::for_each<AllComponents>(ComponentDeserializer(nodeComponent, rhs));
            }
        }
        return true;
    }
}

namespace VWolf {
    
    struct ComponentSerializer {
    public:
        ComponentSerializer(YAML::Emitter& out, VWolf::GameObject& v):
        out(out), v(v) {}
        
        ComponentSerializer(YAML::Emitter& out, const VWolf::GameObject& v):
        out(out), v(v) {}
        
        template <typename T>
        void operator()(T) const {
            if (v.HasComponent<T>()) {
                out << v.GetComponent<T>();
            }
        }
    private:
        YAML::Emitter& out;
        const VWolf::GameObject& v;
    };

    YAML::Emitter& SerializeComponents(YAML::Emitter& out, GameObject& v) {
        out << YAML::Key << componentKey;
        out << YAML::BeginSeq;

        boost::mpl::for_each<AllComponents>(ComponentSerializer(out, v));

        out << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& SerializeComponents(YAML::Emitter& out, const GameObject& v) {
        out << YAML::Key << componentKey;
        out << YAML::BeginSeq;

        boost::mpl::for_each<AllComponents>(ComponentSerializer(out, v));
        
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
