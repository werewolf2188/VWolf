//
//  SphereColliderComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/6/24.
//

#pragma once

enum class SphereColliderConstantKeys {
    Radius
};

static std::map<SphereColliderConstantKeys, const char*> sphereColliderKeys = {
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
            
            rhs.SetRadius(node[sphereColliderKeys[SphereColliderConstantKeys::Radius]].as<float>());
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::SphereColliderComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << sphereColliderKeys[SphereColliderConstantKeys::Radius] << YAML::Value << v.GetRadius();
        out << YAML::EndMap;
        return out;
    }
}
