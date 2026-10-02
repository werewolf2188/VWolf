//
//  OpenGLDevice.h
//  VWolf
//
//  Created by Enrique Moises on 10/2/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Render/RHI/Device.h"
#include "VWolf/Core/Render/RHI/Context.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf::RHI {
    class OpenGLDevice: public Device, public Shareable<OpenGLDevice> {
    public:
        static Ref<OpenGLDevice> GetCurrentDevice();
    public:
        OpenGLDevice();
        ~OpenGLDevice() = default;
    public:
        virtual Ref<RenderContext> CreateRenderContext(const RenderContextSettings& settings) override;
    };
}
