//
//  AudioImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/24/26.
//

#include "AudioImporter.h"

namespace VWolfPup {
    uint32_t AudioImporter::LoadPriority(std::filesystem::path path) const {
        return 3;
    }

    bool AudioImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        VWolf::AudioClip::Load(path, _id);
        return true;
    }

    const std::vector<Extension> AudioImporter::Extensions() const {
        return {
            Extension("Audio MP3", ".mp3")
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(AudioImporter);
}
