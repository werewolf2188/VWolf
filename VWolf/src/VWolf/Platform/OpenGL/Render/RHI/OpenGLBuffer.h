//
//  OpenGLBuffer.h
//  VWolf
//
//  Created by Enrique Moises on 10/13/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Core/Bindable.h"

#include "VWolf/Core/Render/RHI/Buffer.h"

#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderContext.h"

namespace VWolf::RHI {
    class OpenGLBuffer: public Buffer, public Shareable<OpenGLBuffer>, public Bindable {
    public:
        OpenGLBuffer(Weak<OpenGLRenderContext> context, const BufferSettings& settings);
        ~OpenGLBuffer();
        
        virtual void SetData(const SubResource& subResource) override;
        virtual ResourceView GetResourceView() override;
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        const GLuint GetBufferId() { return bufferId; }
        
        void ApplySubResource();
    private:
        GLuint GetType();
        
        GLuint bufferId = 0;
        SubResource subResource;
    };
}
