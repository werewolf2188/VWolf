//
//  GraphicsContext.cpp
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#include "vwpch.h"
#include "GraphicsContext.h"
#include "VWolf/Core/Components/GameObject.h"

namespace VWolf {
    std::vector<Light> GraphicsContext::lights;
    std::vector<Matrix4x4> GraphicsContext::lightSpaces;
    std::vector<LightComponent> GraphicsContext::lights_ex;

    Color GraphicsContext::backgroundColor;

    GraphicsCommandList GraphicsContext::list;

    void GraphicsContext::Reset() {
        lights.clear();
        list.ClearQueue();
    }

    void GraphicsContext::AddLight(LightComponent& lightCo) {
        lights_ex.push_back(lightCo);
        
        TransformComponent& transform = lightCo.GetGameObject()->GetTransform();
        
        Light& light = lightCo.GetLight();
        light.position = Vector4(transform.GetPosition().GetX(), transform.GetPosition().GetY(), transform.GetPosition().GetZ(), 1.0);
        light.direction = (Vector4)transform.GetEulerAngles();
        lights.push_back(light);
        lightSpaces.push_back(light.GetLightSpaceMatrix());
    }
}
