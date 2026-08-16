//
//  MetalBufferSet.h
//  VWolf
//
//  Created by Enrique Moises on 8/19/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/BufferSet.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderContext.h"

namespace VWolf::RHI  {
    class MetalBufferSet: public BufferSet {
    public:
        MetalBufferSet(Weak<MetalRenderContext> context);
        
        virtual void AddBuffers(const std::vector<Ref<Buffer>> buffers) override;
        virtual void SetName(const std::string& name) override;
    protected:
    private:
        
        Weak<MetalRenderContext> context;
    };
}
