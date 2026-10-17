//
//  OpenGLBufferSet.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/13/26.
//

#include "vwpch.h"
#include "OpenGLBufferSet.h"
#include "OpenGLBuffer.h"

namespace VWolf::RHI {
    OpenGLBufferSet::OpenGLBufferSet(Weak<OpenGLRenderContext> context): context(context) {
        
    }

    void OpenGLBufferSet::AddBuffers(const std::vector<Ref<Buffer>> buffers) {
        BufferSet::AddBuffers(buffers);
    }

    void OpenGLBufferSet::Bind() {
        if (buffers.size() <= 0) return;
        
        DynamicAs<OpenGLBuffer>(buffers[0])->Bind();
    }

    void OpenGLBufferSet::Unbind() {
        if (buffers.size() <= 0) return;
        
        DynamicAs<OpenGLBuffer>(buffers[0])->Unbind();
    }
}
