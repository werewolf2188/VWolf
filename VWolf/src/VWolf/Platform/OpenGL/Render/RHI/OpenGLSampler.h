//
//  OpenGLSampler.h
//  VWolf
//
//  Created by Enrique Moises on 10/21/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Core/Bindable.h"

#include "VWolf/Core/Render/RHI/Sampler.h"

#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderContext.h"

namespace VWolf::RHI {
    class OpenGLSampler: public Sampler, public Shareable<OpenGLSampler>, public BaseBindable {
    public:
        OpenGLSampler(Weak<OpenGLRenderContext> context, const SamplerSettings& settings);
        ~OpenGLSampler();
        
        virtual ResourceView GetResourceView() override;
        
        virtual void Bind(uint32_t base) override;
        virtual void Unbind(uint32_t base) override;
        
        const GLuint GetSamplerID() { return m_samplerID; }
    private:
        void Initialize();
        
        GLuint m_samplerID;
        Weak<OpenGLRenderContext> context;
    };
}
