//
//  VMath.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/3/24.
//

#include "vwpch.h"
#include "yaml-cpp/yaml.h"
#include "VWolf/Core/Math/VMath.h"
#include "VMath.h"

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector2Float& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector3Float& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector4Float& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        
        return out;
    }
}
