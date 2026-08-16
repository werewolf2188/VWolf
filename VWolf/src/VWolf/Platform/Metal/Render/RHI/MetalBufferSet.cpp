//
//  MetalBufferSet.cpp
//  VWolf
//
//  Created by Enrique Moises on 8/19/26.
//
#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalBufferSet.h"
#include "MetalBuffer.h"

namespace VWolf::RHI  {
    MetalBufferSet::MetalBufferSet(Weak<MetalRenderContext> context): context(context) {
        
    }

    void MetalBufferSet::AddBuffers(const std::vector<Ref<Buffer>> buffers) {
        BufferSet::AddBuffers(buffers);
    }

    void MetalBufferSet::SetName(const std::string& name) {
        Object::SetName(name);
        
        uint32_t index = 0;
        for(Ref<Buffer> buffer: buffers) {
            std::stringstream ss;
            ss << index;
            buffer->SetName(name + "-" + ss.str());
            index++;
        }
    }
}

#endif
