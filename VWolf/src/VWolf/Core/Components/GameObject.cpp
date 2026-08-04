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
#include <boost/mpl/for_each.hpp>


const std::string componentKey = "Components";

namespace YAML {
    struct ComponentDeserializer {
    public:
        ComponentDeserializer(const Node& node, VWolf::GameObject& rhs):
        node(node), rhs(rhs) {}
        
        template <typename type>
        void operator()(type) const {
            std::string typeName = VWolf::ClassNameCleaner::Current().GetClassName<type>();

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

    GameObject::GameObject(std::string name): Object(UUID::NewUUID()) {
        this->name = name;
    }

    GameObject::GameObject(std::string name, entt::entity handle, Weak<Scene> scene): Object(UUID::NewUUID()), handle(handle), scene(scene) {
        this->name = name;
    }

    GameObject::GameObject(const GameObject& gameObject): Object(gameObject.id) {
        this->name = gameObject.name;
    }

    GameObject::GameObject(GameObject&& gameObject): Object(gameObject.id) {
        name = gameObject.name;
        currentComponents = std::move(gameObject.currentComponents);
        handle = gameObject.handle;
        scene = gameObject.scene;
        
        gameObject.id = UUID::Empty;
        gameObject.name = "";
        gameObject.currentComponents.clear();
        gameObject.handle = entt::null;
    }

    GameObject::~GameObject() {
        ClearCurrentComponents();
    }

    TransformComponent& GameObject::GetTransform() {
        return GetComponent<TransformComponent>();
    }

    void GameObject::AttachToScene(Weak<Scene> scene) {
        if (this->handle == entt::null)
            this->handle = scene.lock()->m_registry.create();
        this->scene = scene;
    }

    void GameObject::SaveComponent(const Ref<Component>& component) {
        currentComponents.push_back(component);
    }

    void GameObject::ClearCurrentComponents() {
        currentComponents.clear();
    }

    void GameObject::CopyComponents(Ref<GameObject> otherGameObject) {
        for (auto refComponent : otherGameObject->currentComponents) {
            if (auto component = refComponent) {
                Ref<Component> newComponent = component->Copy(handle, GetScene()->m_previewRegistry);
                newComponent->SetGameObject(weak_from_this());
                SaveComponent(newComponent);
            }
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
