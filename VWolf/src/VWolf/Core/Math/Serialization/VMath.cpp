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
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Color& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.GetR() << v.GetG() << v.GetB() << v.GetA() << YAML::EndSeq;

        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.GetX() << v.GetY() << YAML::EndSeq;

        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.GetX() << v.GetY() << v.GetZ() << YAML::EndSeq;

        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Vector4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.GetX() << v.GetY() << v.GetZ()  << v.GetZ() << YAML::EndSeq;

        return out;
    }
}
