//
//  MetalRenderContext.cpp
//  VWolf
//
//  Created by Enrique Moises on 8/18/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)

#include "VWolf/Core/Render/Mesh.h"

#include "MetalRenderContext.h"
#include "MetalRenderPass.h"
#include "MetalDevice.h"
#include "MetalCommandQueue.h"
#include "MetalBuffer.h"
#include "MetalBufferSet.h"
#include "MetalTexture.h"
#include "MetalShader.h"
#include "MetalProgram.h"
#include "MetalRenderState.h"

namespace VWolf::RHI  {
    MetalRenderContext::MetalRenderContext(const RenderContextSettings& settings, Ref<MetalDevice> device):
    RenderContext(settings), device(device) {}

    Ref<CommandQueue> MetalRenderContext::CreateCommandQueue() {
        return CreateRef<MetalCommandQueue>(shared_from_this());
    }

    Ref<RenderPattern> MetalRenderContext::CreateRenderPattern(const RenderPatternSettings& settings) {
        return CreateRef<MetalRenderPattern>(weak_from_this(), settings);
    }

    Ref<Buffer> MetalRenderContext::CreateBuffer(const BufferSettings& settings) {
        return CreateRef<MetalBuffer>(weak_from_this(), settings);
    }

    Ref<BufferSet> MetalRenderContext::CreateBufferSet(const std::vector<Ref<Buffer>> buffers) {
        Ref<MetalBufferSet> set = CreateRef<MetalBufferSet>(weak_from_this());
        set->AddBuffers(buffers);
        return set;
    }

    Ref<Texture> MetalRenderContext::CreateTexture(const TextureSettings& settings) {
        return CreateRef<MetalTexture>(weak_from_this(), settings);
    }

    Ref<RenderState> MetalRenderContext::CreateRenderState(const RenderStateSettings& settings) {
        return CreateRef<MetalRenderState>(weak_from_this(), settings);
    }

    Ref<Shader> MetalRenderContext::CreateShader(ShaderType type, const ShaderSettings& settings) {
        return CreateRef<MetalShader>(weak_from_this(), type, settings);
    }

    Ref<Program> MetalRenderContext::CreateProgram(const ProgramSettings& settings) {
        return CreateRef<MetalProgram>(weak_from_this(), settings);
    }
}
#endif
