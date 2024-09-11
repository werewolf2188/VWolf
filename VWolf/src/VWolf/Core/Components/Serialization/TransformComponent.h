//
//  TransformComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/24/23.
//

#pragma once

enum class TransformConstantKeys {
    Position, Rotation, Scale
};

static std::map<TransformConstantKeys, const char*> transformKeys = {
    { TransformConstantKeys::Position, "Position" },
    { TransformConstantKeys::Rotation, "Rotation" },
    { TransformConstantKeys::Scale, "Scale" },
};

namespace YAML {
    template<>
    struct convert<VWolf::TransformComponent>
    {
        static bool decode(const Node& node, VWolf::TransformComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            
            rhs.SetPosition(node[transformKeys[TransformConstantKeys::Position]].as<VWolf::Vector3>());
            rhs.SetEulerAngles(node[transformKeys[TransformConstantKeys::Rotation]].as<VWolf::Vector3>());
            rhs.SetLocalScale(node[transformKeys[TransformConstantKeys::Scale]].as<VWolf::Vector3>());
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::TransformComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << transformKeys[TransformConstantKeys::Position] << YAML::Value << v.GetPosition();
        out << YAML::Key << transformKeys[TransformConstantKeys::Rotation] << YAML::Value << v.GetEulerAngles();
        out << YAML::Key << transformKeys[TransformConstantKeys::Scale] << YAML::Value << v.GetLocalScale();
        out << YAML::EndMap;
        return out;
    }
}
