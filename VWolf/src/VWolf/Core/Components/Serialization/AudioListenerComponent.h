//
//  AudioListenerComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#pragma once

enum class AudioListenerConstantKeys {
    ID
};

static std::map<AudioListenerConstantKeys, const char*> audioListenerKeys = {
    { AudioListenerConstantKeys::ID, "ID" }
};

namespace YAML {
    template<>
    struct convert<VWolf::AudioListenerComponent>
    {
        static bool decode(const Node& node, VWolf::AudioListenerComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            
            if (node[audioListenerKeys[AudioListenerConstantKeys::ID]]) {
                rhs.SetID(node[audioListenerKeys[AudioListenerConstantKeys::ID]].as<VWolf::UUID>());
            }
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::AudioListenerComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << audioListenerKeys[AudioListenerConstantKeys::ID] << YAML::Value << v.GetID();
        out << YAML::EndMap;
        return out;
    }
}
