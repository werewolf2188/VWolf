//
//  OpenGLCommandQueue.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/2/26.
//

#include "vwpch.h"
#include "OpenGLCommandQueue.h"

#include "OpenGLRenderCommandList.h"

namespace VWolf::RHI {

    OpenGLCommandQueue::OpenGLCommandQueue(Weak<OpenGLRenderContext> context): context(context) {
    }

    Weak<Context> OpenGLCommandQueue::GetContext() {
        return context;
    }

    Ref<RenderCommandList> OpenGLCommandQueue::CreateRenderCommandList(Weak<RenderPass> renderPass) {
        return CreateRef<OpenGLRenderCommandList>(weak_from_this(), renderPass);
    }
}
