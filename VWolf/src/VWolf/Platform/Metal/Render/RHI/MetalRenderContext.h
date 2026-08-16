//
//  MetalRenderContext.hpp
//  VWolf
//
//  Created by Enrique Moises on 8/18/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Utils/UUID.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/Context.h"

namespace VWolf::RHI {
    class MetalDevice;
    class MetalBuffer;
    class MetalBufferSet;

    class MetalRenderContext: public RenderContext, public Shareable<MetalRenderContext> {
    public:
        MetalRenderContext(const RenderContextSettings& settings, Ref<MetalDevice> device);
        
        virtual Ref<CommandQueue> CreateCommandQueue() override;
        virtual Ref<RenderPattern> CreateRenderPattern(const RenderPatternSettings& settings) override;
        virtual Ref<RenderState> CreateRenderState(const RenderStateSettings& settings) override;
        virtual Ref<Buffer> CreateBuffer(const BufferSettings& settings) override;
        virtual Ref<BufferSet> CreateBufferSet(const std::vector<Ref<Buffer>> buffers) override;
        virtual Ref<Texture> CreateTexture(const TextureSettings& settings) override;
        virtual Ref<Shader> CreateShader(ShaderType type, const ShaderSettings& settings) override;
        virtual Ref<Program> CreateProgram(const ProgramSettings& settings) override;
        
        Ref<MetalDevice> GetDevice() { return device; }
    protected:
    private:
        Ref<MetalDevice> device;
    };
}
