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
    AudioSourceComponent::AudioSourceComponent(): Component("AudioSource") {
        Initialize();
    }

    AudioSourceComponent::AudioSourceComponent(AudioSourceComponent& audioSource): Component("AudioSource") {
        this->audioFilePath = audioSource.audioFilePath;
        this->mLoop = audioSource.mLoop;
        Initialize();
    }

    AudioSourceComponent::AudioSourceComponent(AudioSourceComponent&& audioSource): Component("AudioSource") {
        this->audioFilePath = audioSource.audioFilePath;
        this->mLoop = audioSource.mLoop;

        audioSource.audioFilePath = "";
        audioSource.mLoop = false;
        Initialize();
    }

    AudioSourceComponent::~AudioSourceComponent() {
        if (sound) {
            if (ma_sound_is_playing(sound))
                CHECKMAERROR(ma_sound_stop(sound));
            ma_sound_uninit(sound);
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
    }

    void AudioSourceComponent::Prepare(TransformComponent& listener, TransformComponent& sourceTransform) {
        listenerPosition = listener.GetPosition();
        listenerDirection = listener.GetEulerAngles();

        ma_engine_listener_set_position(engine, 1, listener.GetPosition().x, listener.GetPosition().y, listener.GetPosition().z);
        ma_engine_listener_set_direction(engine, 1, listener.GetEulerAngles().x, listener.GetEulerAngles().y, listener.GetEulerAngles().z);
        ma_engine_listener_set_world_up(engine, 1, 0, 1, 0);

        if (sound == nullptr && audioFilePath != "") {
            fileTemp = audioFilePath.string();
            sound = (ma_sound*)malloc(sizeof(ma_sound));
            CHECKMAERROR(ma_sound_init_from_file(engine, fileTemp.c_str(), 0, NULL, NULL, sound));
        }
        else if (sound != nullptr) {
            CHECKMAERROR(ma_sound_seek_to_pcm_frame(sound, 0));
        }


        if (sound) {
            // TODO: Fails after playing twice
            ma_sound_set_position(sound, sourceTransform.GetPosition().x, sourceTransform.GetPosition().y, sourceTransform.GetPosition().z);
            ma_sound_set_direction(sound, sourceTransform.GetEulerAngles().x, sourceTransform.GetEulerAngles().y, sourceTransform.GetEulerAngles().z);
            ma_sound_set_looping(sound, mLoop);
            CHECKMAERROR(ma_sound_start(sound));
        }
    }

    void AudioSourceComponent::Update(TransformComponent& listener, TransformComponent& sourceTransform) {
        if (listenerPosition != listener.GetPosition()) {
            ma_engine_listener_set_position(engine, 1, listener.GetPosition().x, listener.GetPosition().y, listener.GetPosition().z);
            listenerPosition = listener.GetPosition();
        }

        if (listenerDirection != listener.GetEulerAngles()) {
            ma_engine_listener_set_direction(engine, 1, listener.GetEulerAngles().x, listener.GetEulerAngles().y, listener.GetEulerAngles().z);
            listenerDirection = listener.GetEulerAngles();
        }

        if (sound) {
            ma_sound_set_position(sound, sourceTransform.GetPosition().x, sourceTransform.GetPosition().y, sourceTransform.GetPosition().z);
            ma_sound_set_direction(sound, sourceTransform.GetEulerAngles().x, sourceTransform.GetEulerAngles().y, sourceTransform.GetEulerAngles().z);
        }
    }

    void AudioSourceComponent::End() {
        if (sound)
            CHECKMAERROR(ma_sound_stop(sound));
    }

    void AudioSourceComponent::OnInspector() {
        AudioSourceComponent::componentInspector->OnInspector(this);
    }

    Component* AudioSourceComponent::Copy(entt::entity& handle, entt::registry& registry) {
        AudioSourceComponent& component = registry.emplace<AudioSourceComponent>(handle, *this);
        return &component;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(AudioSourceComponent);
}
