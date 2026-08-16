//
//  MCommand.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/5/24.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Render/RHI/CommandQueue.h"
#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderContext.h"

namespace VWolf::RHI  {
    class MetalCommandQueue: public CommandQueue, public Shareable<MetalCommandQueue> {
    public:
        MetalCommandQueue(Ref<MetalRenderContext> context);
    public:
        Ref<MTL::CommandQueue> GetCommandQueue() { return commandQueue; }
        
        virtual Weak<Context> GetContext() override;
        virtual Ref<RenderCommandList> CreateRenderCommandList(Weak<RenderPass> renderPass) override;
        virtual void SetName(const std::string& name) override;
    private:
        Ref<MTL::CommandQueue> commandQueue;
        Weak<MetalRenderContext> context;
    };
}
