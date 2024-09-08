//
//  LightComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/25/23.
//

#pragma once

enum class LightComponentKeys {
    Light,
    LightColor, LightPosition, LightDirection, LightStrength, LightCutOff, LightExponent, LightType
};

static std::map<LightComponentKeys, const char*> lightKeys = {
    { LightComponentKeys::Light, "Light" },
    { LightComponentKeys::LightColor, "color" },
    { LightComponentKeys::LightPosition, "position" },
    { LightComponentKeys::LightDirection, "direction" },
    { LightComponentKeys::LightStrength, "strength" },
    { LightComponentKeys::LightCutOff, "cutOff" },
    { LightComponentKeys::LightExponent, "exponent" },
    { LightComponentKeys::LightType, "type" }
};

namespace YAML {

    template<>
    struct convert<VWolf::Light>
    {
        static bool decode(const Node& node, VWolf::Light& rhs)
        {
            if (!node.IsMap())
                return false;
            
            rhs.type = (VWolf::Light::LightType)node[lightKeys[LightComponentKeys::LightType]].as<unsigned int>();
            rhs.color = node[lightKeys[LightComponentKeys::LightColor]].as<VWolf::Vector4>();
            rhs.position = node[lightKeys[LightComponentKeys::LightPosition]].as<VWolf::Vector4>();
            rhs.direction = node[lightKeys[LightComponentKeys::LightDirection]].as<VWolf::Vector4>();
            rhs.strength = node[lightKeys[LightComponentKeys::LightStrength]].as<VWolf::Vector4>();
            rhs.cutOff = node[lightKeys[LightComponentKeys::LightCutOff]].as<float>();
            rhs.exponent = node[lightKeys[LightComponentKeys::LightExponent]].as<float>();

            return true;
        }
    };

    template<>
    struct convert<VWolf::LightComponent>
    {
        static bool decode(const Node& node, VWolf::LightComponent& rhs)
        {
            if (!node.IsMap())
                return false;

            rhs.SetLight(node[lightKeys[LightComponentKeys::Light]].as<VWolf::Light>());

            return true;
        }
    };
}

namespace VWolf {

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Light& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << lightKeys[LightComponentKeys::LightColor] << YAML::Value << v.color;
        out << YAML::Key << lightKeys[LightComponentKeys::LightPosition] << YAML::Value << v.position;
        out << YAML::Key << lightKeys[LightComponentKeys::LightDirection] << YAML::Value << v.direction;
        out << YAML::Key << lightKeys[LightComponentKeys::LightStrength] << YAML::Value << v.strength;
        out << YAML::Key << lightKeys[LightComponentKeys::LightCutOff] << YAML::Value << v.cutOff;
        out << YAML::Key << lightKeys[LightComponentKeys::LightExponent] << YAML::Value << v.exponent;
        out << YAML::Key << lightKeys[LightComponentKeys::LightType] << YAML::Value << (unsigned int)v.type;
        out << YAML::EndMap;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::LightComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << lightKeys[LightComponentKeys::Light];
        out << v.GetLight();
        out << YAML::EndMap;
        return out;
    }
}
