//
//  AudioListenerComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#pragma once

enum class AudioListenerConstantKeys {
};

static std::map<AudioListenerConstantKeys, const char*> audioListenerKeys = {
};

namespace YAML {
    template<>
    struct convert<VWolf::AudioListenerComponent>
    {
        static bool decode(const Node& node, VWolf::AudioListenerComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::AudioListenerComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::EndMap;
        return out;
    }
}
