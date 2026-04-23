//
//  SphereColliderComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/6/24.
//

#pragma once

enum class SphereColliderConstantKeys {
    ID, Radius
};

static std::map<SphereColliderConstantKeys, const char*> sphereColliderKeys = {
    { SphereColliderConstantKeys::ID, "ID" },
    { SphereColliderConstantKeys::Radius, "Radius" }
};

namespace YAML {
    template<>
    struct convert<VWolf::SphereColliderComponent>
    {
        static bool decode(const Node& node, VWolf::SphereColliderComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            
            if (node[sphereColliderKeys[SphereColliderConstantKeys::ID]]) {
                rhs.SetID(node[sphereColliderKeys[SphereColliderConstantKeys::ID]].as<VWolf::UUID>());
            }
            rhs.SetRadius(node[sphereColliderKeys[SphereColliderConstantKeys::Radius]].as<float>());
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::SphereColliderComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << sphereColliderKeys[SphereColliderConstantKeys::ID] << YAML::Value << v.GetID();
        out << YAML::Key << sphereColliderKeys[SphereColliderConstantKeys::Radius] << YAML::Value << v.GetRadius();
        out << YAML::EndMap;
        return out;
    }
}
