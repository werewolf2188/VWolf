//
//  Device.h
//  VWolf
//
//  Created by Enrique Moises on 8/17/26.
//

#pragma once
#include "Object.h"

namespace VWolf::RHI  {
    
    class RenderContextSettings;
    class RenderContext;
    
    class Device: public Object {
    public:
        virtual Ref<RenderContext> CreateRenderContext(const RenderContextSettings& settings) = 0;
    protected:
    private:
    };
}
