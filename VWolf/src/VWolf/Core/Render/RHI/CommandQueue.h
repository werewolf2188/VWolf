//
//  CommandQueue.h
//  VWolf
//
//  Created by Enrique Moises on 8/17/26.
//

#pragma once
#include "Object.h"

namespace VWolf::RHI  {
    
    class Context;
    class RenderCommandList;
    class RenderPass;

    class CommandQueue: public Object {
    public:
        virtual Weak<Context> GetContext() = 0;
        
        virtual Ref<RenderCommandList> CreateRenderCommandList(Weak<RenderPass> renderPass) = 0;
    protected:
    private:
    };
}
