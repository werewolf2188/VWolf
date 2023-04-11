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
        ~LightComponent();
    public:
        Light& GetLight(TransformComponent component);
        Light& GetLight() { return light; }

        virtual void OnInspector() override;
    private:
        Light light;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(LightComponent);
    };
}
