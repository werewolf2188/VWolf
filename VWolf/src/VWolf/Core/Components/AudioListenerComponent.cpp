//
//  AudioListenerComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#include "vwpch.h"
#include "AudioListenerComponent.h"

namespace VWolf {
    AudioListenerComponent::AudioListenerComponent(): Component("AudioListener") {}

    AudioListenerComponent::AudioListenerComponent(AudioListenerComponent& audioListener): Component("AudioListener") {}

    AudioListenerComponent::AudioListenerComponent(AudioListenerComponent&& audioListener): Component("AudioListener") {}

    AudioListenerComponent::~AudioListenerComponent() {}

    void AudioListenerComponent::OnInspector() {
        AudioListenerComponent::componentInspector->OnInspector(this);
    }

    Component* AudioListenerComponent::Copy(entt::entity& handle, entt::registry& registry) {
        AudioListenerComponent& component = registry.emplace<AudioListenerComponent>(handle, *this);
        return &component;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(AudioListenerComponent);
}
