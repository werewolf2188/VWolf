//
//  GameObject.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/24/23.
//

#pragma once

#define COMPONENT_KEY(T) T
#define COMPONENT_KEY_VALUE(T) { GameObjectConstantKeys::T, #T }

#define SERIALIZE_COMPONENT(T) \
if (v.HasComponent<T>()) {\
    out << YAML::BeginMap;\
    out << YAML::Key << gameObjectKeys[GameObjectConstantKeys::T];\
    out << v.GetComponent<T>();\
    out << YAML::EndMap;\
}

#define DESERIALIZE_COMPONENT(T) \
if (component[gameObjectKeys[GameObjectConstantKeys::T]]) { \
    VWolf::T temp = component[gameObjectKeys[GameObjectConstantKeys::T]] \
        .as<VWolf::T>(); \
    rhs.AddComponent<VWolf::T>(temp); \
}

enum class GameObjectConstantKeys {
    Name, Components,
    COMPONENT_KEY(TransformComponent),
    COMPONENT_KEY(ShapeRendererComponent),
    COMPONENT_KEY(MeshFilterComponent),
    COMPONENT_KEY(MeshRendererComponent),
    COMPONENT_KEY(LightComponent),
    COMPONENT_KEY(CameraComponent),
    COMPONENT_KEY(RigidBodyComponent),
    COMPONENT_KEY(MeshColliderComponent),
    COMPONENT_KEY(SphereColliderComponent),
    COMPONENT_KEY(BoxColliderComponent)
};

static std::map<GameObjectConstantKeys, const char*> gameObjectKeys = {
    { GameObjectConstantKeys::Name, "Name" },
    { GameObjectConstantKeys::Components, "Components" },
    COMPONENT_KEY_VALUE(TransformComponent),
    COMPONENT_KEY_VALUE(ShapeRendererComponent),
    COMPONENT_KEY_VALUE(MeshFilterComponent),
    COMPONENT_KEY_VALUE(MeshRendererComponent),
    COMPONENT_KEY_VALUE(LightComponent),
    COMPONENT_KEY_VALUE(CameraComponent),
    COMPONENT_KEY_VALUE(RigidBodyComponent),
    COMPONENT_KEY_VALUE(MeshColliderComponent),
    COMPONENT_KEY_VALUE(SphereColliderComponent),
    COMPONENT_KEY_VALUE(BoxColliderComponent)
};

namespace YAML {
    template<>
    struct convert<VWolf::GameObject>
    {
        static bool decode(const Node& node, VWolf::GameObject& rhs)
        {
            if (!node.IsMap() ||
                !node[gameObjectKeys[GameObjectConstantKeys::Name]])
                return false;

            rhs.AttachToScene(currentScene);
            rhs.SetName(node[gameObjectKeys[GameObjectConstantKeys::Name]].as<std::string>());

            if (node[gameObjectKeys[GameObjectConstantKeys::Components]]) {
                for (auto component: node[gameObjectKeys[GameObjectConstantKeys::Components]]) {

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
                }
            }
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::GameObject& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << gameObjectKeys[GameObjectConstantKeys::Name] << YAML::Value << v.GetName();
        out << YAML::Key << gameObjectKeys[GameObjectConstantKeys::Components];
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

        out << YAML::EndSeq;
        out << YAML::EndMap;
        return out;
    }
}
