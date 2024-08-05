//
//  AudioSourceComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#pragma once

enum class AudioSourceConstantKeys {
    AudioFile, Loop
};

static std::map<AudioSourceConstantKeys, const char*> audioSourceKeys = {
    { AudioSourceConstantKeys::AudioFile, "AudioFile" },
    { AudioSourceConstantKeys::Loop, "Loop" }
};

namespace YAML {
    template<>
    struct convert<VWolf::AudioSourceComponent>
    {
        static bool decode(const Node& node, VWolf::AudioSourceComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            rhs.SetAudioFile(node[audioSourceKeys[AudioSourceConstantKeys::AudioFile]].as<std::string>());
            rhs.SetLoop(node[audioSourceKeys[AudioSourceConstantKeys::Loop]].as<bool>());
            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::AudioSourceComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << audioSourceKeys[AudioSourceConstantKeys::AudioFile] << YAML::Value << v.GetAudioFile().string();
        out << YAML::Key << audioSourceKeys[AudioSourceConstantKeys::Loop] << YAML::Value << v.GetLoop();
        out << YAML::EndMap;
        return out;
    }
}
