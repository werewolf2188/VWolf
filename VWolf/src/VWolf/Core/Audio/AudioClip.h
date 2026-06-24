//
//  AudioClip.h
//  VWolf
//
//  Created by Enrique Moises on 6/24/26.
//

#pragma once

#include "VWolf/Core/Object.h"
#include "VWolf/Core/Math/VMath.h"

struct ma_sound;
struct ma_engine;

namespace VWolf {
    class AudioClip: public Object {
    public:
        AudioClip();
        AudioClip(std::filesystem::path path, UUID _id);
        AudioClip(const AudioClip& clip);
        
        ~AudioClip();
    public:
        AudioClip& operator=(const AudioClip& clip);
    public:
        static bool Load(std::filesystem::path path, UUID _id);
    private:
        bool Initialize(ma_engine* engine);
    private:
        std::filesystem::path path;
        ma_sound* sound = nullptr;
        
        friend class AudioSourceComponent;
    };
}
