//
//  GraphicsContext.h
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Color.h"
#include "VWolf/Core/Math/VMath.h"
#include "GraphicsCommands.h"
#include "VWolf/Core/Components/Components.h"

namespace VWolf {
    // This class will help to separate the logic to add the information that's not related to the commands but it needs to be sent to the GPU
    // Such as the lights, the clear color, etc... Mostly repeated code among the internal graphics classes.
    // There's a lot of repeated code that should not be inside those classes but be preprocessed before getting to the draw calls.
    class GraphicsContext {
    public:
        static void Reset();
        static void AddLight(LightComponent& light);
    public:
        static GraphicsCommandList& GetList() { return list; }
        static Color GetBackgroundColor() { return backgroundColor; }
        static void SetClearColor(Color _backgroundColor) { backgroundColor = _backgroundColor; }
        static std::vector<LightComponent>& GetLights_Ex() { return lights_ex; }
        static std::vector<Light>& GetLights() { return lights; }
        static std::vector<Matrix4x4>& GetLightsSpaces() { return lightSpaces; }
    private:
        static GraphicsCommandList list;
        static std::vector<LightComponent> lights_ex;
        static std::vector<Light> lights;
        static std::vector<Matrix4x4> lightSpaces;
        static Color backgroundColor;
    };
}
