//
//  AudioSourceComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Utils/GenericSerialization.h"
#include "VWolf/Core/Audio/AudioClip.h"

struct ma_engine;
struct ma_sound;

namespace VWolf {
    class TransformComponent;

    class AudioSourceComponent: public Component {
    public:
        AudioSourceComponent();
        AudioSourceComponent(const AudioSourceComponent& audioSource);
        AudioSourceComponent(AudioSourceComponent&& audioSource);
        ~AudioSourceComponent();
    public:
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        bool& GetLoop() { return mLoop; }
        void SetLoop(bool loop) { mLoop = loop; }
    public:
        void SetAudioClip(Ref<AudioClip> clip);
        Ref<AudioClip> GetAudioClip();
#ifdef VWOLF_CORE
    public:
        void Prepare(TransformComponent& listener, TransformComponent& sourceTransform);
        void Update(TransformComponent& listener, TransformComponent& sourceTransform);
        void End();
#endif
    private:
        void Initialize();
    private:
        UUID audioClipId;
        bool mLoop;
        BOOST_DESCRIBE_CLASS(AudioSourceComponent, (Component), (), (id), (audioClipId, mLoop))

        ma_engine* engine = nullptr;
        
        Ref<AudioClip> audioClip;

        Vector3 listenerPosition, listenerDirection;
        VWOLF_SERIALIZATION_FRIENDS(AudioSourceComponent)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::AudioSourceComponent)
}
