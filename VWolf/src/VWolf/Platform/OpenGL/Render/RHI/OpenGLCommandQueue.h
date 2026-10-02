//
//  OpenGLCommandQueue.h
//  VWolf
//
//  Created by Enrique Moises on 10/2/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Render/RHI/CommandQueue.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderContext.h"

namespace VWolf::RHI {
    class OpenGLCommandQueue: public CommandQueue, public Shareable<OpenGLCommandQueue> {
    public:
        OpenGLCommandQueue(Weak<OpenGLRenderContext> context);
    public:
        
        virtual Weak<Context> GetContext() override;
        virtual Ref<RenderCommandList> CreateRenderCommandList(Weak<RenderPass> renderPass) override;
    private:
        Weak<OpenGLRenderContext> context;
    };
}
