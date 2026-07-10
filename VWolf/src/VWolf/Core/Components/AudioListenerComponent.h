//
//  AudioListenerComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/31/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {
    class AudioListenerComponent: public Component {
    public:
        AudioListenerComponent();
        AudioListenerComponent(const AudioListenerComponent& audioListener);
        AudioListenerComponent(AudioListenerComponent&& audioListener);
        ~AudioListenerComponent();
    public:
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
        BOOST_DESCRIBE_CLASS(AudioListenerComponent, (Component), (), (id), ())
        
        VWOLF_SERIALIZATION_FRIENDS(AudioListenerComponent)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::AudioListenerComponent)
}
