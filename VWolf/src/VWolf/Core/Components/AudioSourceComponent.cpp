//
//  AudioSourceComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#include "vwpch.h"
#include "miniaudio.h"
#include "VWolf/Core/Audio/Core.h"
#include "AudioSourceComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"

namespace VWolf {
    AudioSourceComponent::AudioSourceComponent(): Component("AudioSource"), audioClipId(UUID::Empty), mLoop(false) {
        Initialize();
    }

    AudioSourceComponent::AudioSourceComponent(const AudioSourceComponent& audioSource):
    Component("AudioSource", audioSource.id) {
        this->audioClipId = audioSource.audioClipId;
        this->mLoop = audioSource.mLoop;
        Initialize();
    }

    AudioSourceComponent::AudioSourceComponent(AudioSourceComponent&& audioSource):
    Component("AudioSource", audioSource.id) {
        this->audioClipId = audioSource.audioClipId;
        this->mLoop = audioSource.mLoop;

        audioSource.audioClipId = UUID::Empty;
        audioSource.mLoop = false;
        Initialize();
    }

    void AudioSourceComponent::SetAudioClip(Ref<AudioClip> clip) {
        this->audioClip = clip;
        this->audioClipId = clip->GetID();
    }

    Ref<AudioClip> AudioSourceComponent::GetAudioClip() {
        return audioClip;
    }

    AudioSourceComponent::~AudioSourceComponent() {
        if (audioClip && audioClip->sound) {
            if (ma_sound_is_playing(audioClip->sound))
                CHECKMAERROR(ma_sound_stop(audioClip->sound));
        }
        ma_engine_uninit(engine);
    }

    void AudioSourceComponent::Initialize() {
        if (engine != nullptr) return;
        ma_engine_config engineConfig;

        engineConfig = ma_engine_config_init();
        engineConfig.listenerCount = 1;

        engine = (ma_engine*)malloc(sizeof(ma_engine));
        CHECKMAERROR(ma_engine_init(&engineConfig, engine));
        
        if (audioClipId != UUID::Empty) {
            audioClip = ObjectResourceManager::Get<AudioClip>(audioClipId);
        }
    }

    void AudioSourceComponent::Prepare(TransformComponent& listener, TransformComponent& sourceTransform) {
        listenerPosition = listener.GetPosition();
        listenerDirection = listener.GetEulerAngles();

        ma_engine_listener_set_position(engine, 1, listener.GetPosition().GetX(), listener.GetPosition().GetY(), listener.GetPosition().GetZ());
        ma_engine_listener_set_direction(engine, 1, listener.GetEulerAngles().GetX(), listener.GetEulerAngles().GetY(), listener.GetEulerAngles().GetZ());
        ma_engine_listener_set_world_up(engine, 1, 0, 1, 0);

        if (audioClip) {
            audioClip->Initialize(engine);
            
            if (audioClip->sound == nullptr) return;
            
            ma_sound_set_position(audioClip->sound, sourceTransform.GetPosition().GetX(), sourceTransform.GetPosition().GetY(), sourceTransform.GetPosition().GetZ());
            ma_sound_set_direction(audioClip->sound, sourceTransform.GetEulerAngles().GetX(), sourceTransform.GetEulerAngles().GetY(), sourceTransform.GetEulerAngles().GetZ());
            ma_sound_set_looping(audioClip->sound, mLoop);
            CHECKMAERROR(ma_sound_start(audioClip->sound));
        }
    }

    void AudioSourceComponent::Update(TransformComponent& listener, TransformComponent& sourceTransform) {
        if (listenerPosition != listener.GetPosition()) {
            ma_engine_listener_set_position(engine, 1, listener.GetPosition().GetX(), listener.GetPosition().GetY(), listener.GetPosition().GetZ());
            listenerPosition = listener.GetPosition();
        }

        if (listenerDirection != listener.GetEulerAngles()) {
            ma_engine_listener_set_direction(engine, 1, listener.GetEulerAngles().GetX(), listener.GetEulerAngles().GetY(), listener.GetEulerAngles().GetZ());
            listenerDirection = listener.GetEulerAngles();
        }

        if (audioClip && audioClip->sound) {
            ma_sound_set_position(audioClip->sound, sourceTransform.GetPosition().GetX(), sourceTransform.GetPosition().GetY(), sourceTransform.GetPosition().GetZ());
            ma_sound_set_direction(audioClip->sound, sourceTransform.GetEulerAngles().GetX(), sourceTransform.GetEulerAngles().GetY(), sourceTransform.GetEulerAngles().GetZ());
        }
    }

    void AudioSourceComponent::End() {
        if (audioClip && audioClip->sound)
            CHECKMAERROR(ma_sound_stop(audioClip->sound));
    }

    void AudioSourceComponent::OnInspector() {
        AudioSourceComponent::componentInspector->OnInspector(this);
    }

    Component* AudioSourceComponent::Copy(entt::entity& handle, entt::registry& registry) {
        AudioSourceComponent& component = registry.emplace<AudioSourceComponent>(handle, *this);
        return &component;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(AudioSourceComponent);

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(AudioSourceComponent, "AudioSourceComponent")
}
