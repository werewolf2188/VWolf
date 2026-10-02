//
//  OpenGLBufferSet.h
//  VWolf
//
//  Created by Enrique Moises on 10/13/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Core/Bindable.h"

#include "VWolf/Core/Render/RHI/BufferSet.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderContext.h"

namespace VWolf::RHI {
    class OpenGLBufferSet: public BufferSet, public Bindable {
    public:
        OpenGLBufferSet(Weak<OpenGLRenderContext> context);
        
        virtual void AddBuffers(const std::vector<Ref<Buffer>> buffers) override;
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        void ApplySubResource();
    protected:
    private:
        
        Weak<OpenGLRenderContext> context;
    };
}
