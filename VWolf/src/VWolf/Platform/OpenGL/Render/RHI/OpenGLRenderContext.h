//
//  OpenGLRenderContext.h
//  VWolf
//
//  Created by Enrique Moises on 10/2/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Utils/UUID.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"

#include "VWolf/Core/Render/RHI/Context.h"

namespace VWolf::RHI {
    class OpenGLDevice;

    class OpenGLRenderContext: public RenderContext, public Shareable<OpenGLRenderContext> {
    public:
        OpenGLRenderContext(const RenderContextSettings& settings, Ref<OpenGLDevice> device);
        
        virtual Ref<CommandQueue> CreateCommandQueue() override;
        virtual Ref<RenderPattern> CreateRenderPattern(const RenderPatternSettings& settings) override;
        virtual Ref<RenderState> CreateRenderState(const RenderStateSettings& settings) override;
        virtual Ref<Buffer> CreateBuffer(const BufferSettings& settings) override;
        virtual Ref<BufferSet> CreateBufferSet(const std::vector<Ref<Buffer>> buffers) override;
        virtual Ref<Texture> CreateTexture(const TextureSettings& settings) override;
        virtual Ref<Shader> CreateShader(ShaderType type, const ShaderSettings& settings) override;
        virtual Ref<Program> CreateProgram(const ProgramSettings& settings) override;
        virtual Ref<Sampler> CreateSampler(const SamplerSettings& settings) override;
        
        Ref<OpenGLDevice> GetDevice() { return device; }
    protected:
    private:
        Ref<OpenGLDevice> device;
    };
}
