//
//  AudioClip.cpp
//  VWolf
//
//  Created by Enrique Moises on 6/24/26.
//

#include "vwpch.h"
#include "miniaudio.h"
#include "AudioClip.h"

#include "VWolf/Core/Audio/Core.h"

namespace VWolf {
    AudioClip::AudioClip(): Object(UUID::NewUUID()) {
        name = "";
    }

    AudioClip::AudioClip(std::filesystem::path path, UUID _id): Object(_id), path(path) {
        name = path.stem();
    }

    AudioClip::AudioClip(const AudioClip& clip): Object(clip.GetID()),  path(clip.path) {
        name = clip.GetName();
    }

    AudioClip::~AudioClip() {
        ma_sound_uninit(sound);
    }

    AudioClip& AudioClip::operator=(const AudioClip &clip) {
        this->id = clip.id;
        this->path = clip.path;
        this->name = clip.GetName();
        return *this;
    }

    bool AudioClip::Load(std::filesystem::path path, UUID _id) {
        ObjectResourceManager::AddObject(_id, CreateRef<AudioClip>(path, _id));
        return true;
    }

    bool AudioClip::Initialize(ma_engine* engine) {
        if (sound == nullptr && path != "") {
            std::string fileTemp = path.string();
            sound = (ma_sound*)malloc(sizeof(ma_sound));
            CHECKMAERROR(ma_sound_init_from_file(engine, fileTemp.c_str(), 0, NULL, NULL, sound));
        }
        else if (sound != nullptr) {
//            CHECKMAERROR(ma_sound_seek_to_pcm_frame(sound, 0));
        }
        return true;
    }
}
