//
//  AudioListenerComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#pragma once

#include "BaseComponent.h"

namespace VWolf {
    class AudioListenerComponent: public Component {
    public:
        AudioListenerComponent();
        AudioListenerComponent(AudioListenerComponent& audioListener);
        AudioListenerComponent(AudioListenerComponent&& audioListener);
        ~AudioListenerComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(AudioListenerComponent);
    };
}
