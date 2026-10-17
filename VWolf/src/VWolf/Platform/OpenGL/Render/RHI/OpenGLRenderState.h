//
//  OpenGLRenderState.h
//  VWolf
//
//  Created by Enrique Moises on 10/12/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Core/Bindable.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Render/RHI/RenderState.h"

namespace VWolf::RHI {
    class OpenGLRenderState: public RenderState, public Bindable {
    public:
        OpenGLRenderState(Weak<RenderContext> context, const RenderStateSettings& settings);
        ~OpenGLRenderState();
        void SetConfiguration() const;
        
        void Bind() override;
		void Unbind() override;
    private:
        GLuint GetBlendFunction(RHI::BlendFunction function) const;
        
        void SetRasterization() const;
        void SetBlend() const;
        void SetDepthStencil() const;
    };
}
