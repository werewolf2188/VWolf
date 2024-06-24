//
//  BoxColliderComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/20/24.
//

#pragma once

enum class BoxColliderConstantKeys {
//    Mass, Drag, AngularDrag, UseGravity, BodyType
};

static std::map<BoxColliderConstantKeys, const char*> boxColliderKeys = {
//    { RigidBodyConstantKeys::Mass, "Mass" },
//    { RigidBodyConstantKeys::Drag, "Drag" },
//    { RigidBodyConstantKeys::AngularDrag, "AngularDrag" },
//    { RigidBodyConstantKeys::UseGravity, "UseGravity" },
//    { RigidBodyConstantKeys::BodyType, "BodyType" }
};

namespace YAML {
    template<>
    struct convert<VWolf::BoxColliderComponent>
    {
        static bool decode(const Node& node, VWolf::BoxColliderComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            
//            rhs.SetMass(node[rigidBodyKeys[RigidBodyConstantKeys::Mass]].as<float>());
//            rhs.SetDrag(node[rigidBodyKeys[RigidBodyConstantKeys::Drag]].as<float>());
//            rhs.SetAngularDrag(node[rigidBodyKeys[RigidBodyConstantKeys::AngularDrag]].as<float>());
//            rhs.SetUseGravity(node[rigidBodyKeys[RigidBodyConstantKeys::UseGravity]].as<bool>());
//            rhs.SetBodyType(node[rigidBodyKeys[RigidBodyConstantKeys::BodyType]].as<uint32_t>());
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::BoxColliderComponent& v)
    {
        out << YAML::BeginMap;
//        out << YAML::Key << rigidBodyKeys[RigidBodyConstantKeys::Mass] << YAML::Value << v.GetMass();
//        out << YAML::Key << rigidBodyKeys[RigidBodyConstantKeys::Drag] << YAML::Value << v.GetDrag();
//        out << YAML::Key << rigidBodyKeys[RigidBodyConstantKeys::AngularDrag] << YAML::Value << v.GetAngularDrag();
//        out << YAML::Key << rigidBodyKeys[RigidBodyConstantKeys::UseGravity] << YAML::Value << v.GetUseGravity();
//        out << YAML::Key << rigidBodyKeys[RigidBodyConstantKeys::BodyType] << YAML::Value << v.GetBodyType();
        out << YAML::EndMap;
        return out;
    }
}

