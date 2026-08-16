//
//  MCommand.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/5/24.
//
#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalCommandQueue.h"
#include "MetalRenderCommandList.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"
#include "MetalDevice.h"

namespace VWolf::RHI  {
    MetalCommandQueue::MetalCommandQueue(Ref<MetalRenderContext> context): context(context->weak_from_this()) {
        commandQueue = CreateMetalRef<MTL::CommandQueue>(context->GetDevice()->GetDevice()->newCommandQueue());
    }

    Weak<Context> MetalCommandQueue::GetContext() {
        return context;
    }

    Ref<RenderCommandList> MetalCommandQueue::CreateRenderCommandList(Weak<RenderPass> renderPass) {
        return CreateRef<MetalRenderCommandList>(weak_from_this(), renderPass);
    }

    void MetalCommandQueue::SetName(const std::string& name) {
        Object::SetName(name);
        if (!name.empty())
            commandQueue->setLabel(NSSTRINGFROM(name.data()));
    }
}

#endif
