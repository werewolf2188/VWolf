//
//  OpenGLRenderContext.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/2/26.
//

#include "vwpch.h"
#include "OpenGLRenderContext.h"
#include "OpenGLCommandQueue.h"
#include "OpenGLRenderPass.h"
#include "OpenGLTexture.h"
#include "OpenGLShader.h"
#include "OpenGLProgram.h"
#include "OpenGLRenderState.h"
#include "OpenGLBuffer.h"
#include "OpenGLBufferSet.h"
#include "OpenGLSampler.h"

#include "OpenGLDevice.h"

namespace VWolf::RHI {
    OpenGLRenderContext::OpenGLRenderContext(const RenderContextSettings& settings, Ref<OpenGLDevice> device):
    RenderContext(settings), device(device) {
    }

    Ref<CommandQueue> OpenGLRenderContext::CreateCommandQueue() {
        return CreateRef<OpenGLCommandQueue>(weak_from_this());
    }

    Ref<RenderPattern> OpenGLRenderContext::CreateRenderPattern(const RenderPatternSettings& settings) {
        return CreateRef<OpenGLRenderPattern>(weak_from_this(), settings);
    }

    Ref<RenderState> OpenGLRenderContext::CreateRenderState(const RenderStateSettings& settings) {
        return CreateRef<OpenGLRenderState>(weak_from_this(), settings);
    }
    
    Ref<Buffer> OpenGLRenderContext::CreateBuffer(const BufferSettings& settings) {
        return CreateRef<OpenGLBuffer>(weak_from_this(), settings);
    }

    Ref<BufferSet> OpenGLRenderContext::CreateBufferSet(const std::vector<Ref<Buffer>> buffers) {
        Ref<OpenGLBufferSet> set = CreateRef<OpenGLBufferSet>(weak_from_this());
        set->AddBuffers(buffers);
        return set;
    }

    Ref<Texture> OpenGLRenderContext::CreateTexture(const TextureSettings& settings) {
        return CreateRef<OpenGLTexture>(weak_from_this(), settings);
    }

    Ref<Shader> OpenGLRenderContext::CreateShader(ShaderType type, const ShaderSettings& settings) {
        return CreateRef<OpenGLShader>(weak_from_this(), type, settings);
    }

    Ref<Program> OpenGLRenderContext::CreateProgram(const ProgramSettings& settings) {
        return CreateRef<OpenGLProgram>(weak_from_this(), settings);
    }

    Ref<Sampler> OpenGLRenderContext::CreateSampler(const SamplerSettings& settings) {
        return CreateRef<OpenGLSampler>(weak_from_this(), settings);
    }
}
