//
//  LightComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "BaseComponent.h"

#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Render/RenderStructs.h"

namespace VWolf {
    class TransformComponent;

    class LightComponent: public Component {
    public:
        LightComponent();
        LightComponent(LightComponent& light);
        LightComponent(LightComponent&& light);
        ~LightComponent();
    public:
        Light& GetLight(TransformComponent component);
        Light& GetLight() { return light; }
        void SetLight(Light light) { this->light = light; }

        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        LightComponent& operator=(LightComponent& light);
    private:
        Light light;
        VWOLF_COMPONENT_INSPECTOR_DEFINE(LightComponent);
        BOOST_DESCRIBE_CLASS(LightComponent, (Component), (), (id), (light))
        
        VWOLF_SERIALIZATION_FRIENDS(LightComponent)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::LightComponent)
}
