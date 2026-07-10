//
//  AudioListenerComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#include "vwpch.h"
#include "AudioListenerComponent.h"

namespace VWolf {
    AudioListenerComponent::AudioListenerComponent():
    Component(ClassNameCleaner::Current().GetClassName<AudioListenerComponent>()) {}

    AudioListenerComponent::AudioListenerComponent(const AudioListenerComponent& audioListener):
    Component(ClassNameCleaner::Current().GetClassName<AudioListenerComponent>(), audioListener.id) {}

    AudioListenerComponent::AudioListenerComponent(AudioListenerComponent&& audioListener):
    Component(ClassNameCleaner::Current().GetClassName<AudioListenerComponent>(), audioListener.id) {}

    AudioListenerComponent::~AudioListenerComponent() {}

    Component* AudioListenerComponent::Copy(entt::entity& handle, entt::registry& registry) {
        AudioListenerComponent& component = registry.emplace<AudioListenerComponent>(handle, *this);
        return &component;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(AudioListenerComponent, "AudioListenerComponent")
}
