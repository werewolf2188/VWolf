//
//  GraphicsContext.cpp
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#include "vwpch.h"
#include "GraphicsContext.h"

namespace VWolf {
    std::vector<Light> GraphicsContext::lights;
    std::vector<Matrix4x4> GraphicsContext::lightSpaces;

    Color GraphicsContext::backgroundColor;

    GraphicsCommandList GraphicsContext::list;

    void GraphicsContext::Reset() {
        lights.clear();
        list.ClearQueue();
    }

    void GraphicsContext::AddLight(Light& light, Vector3 position, Vector3 eulerAngles) {
        
        light.position = Vector4(position.GetX(), position.GetY(), position.GetZ(), 1.0);
        light.direction = (Vector4)eulerAngles;
        lights.push_back(light);
        lightSpaces.push_back(light.GetLightSpaceMatrix());
    }
}
