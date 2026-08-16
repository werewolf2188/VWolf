//
//  Context.h
//  VWolf
//
//  Created by Enrique Moises on 8/18/26.
//

#pragma once

#include "Types.h"
#include "Object.h"

namespace VWolf {
    class Mesh;
}

namespace VWolf::RHI  {
    class RenderPattern;
    class RenderPatternSettings;
    class RenderState;
    class RenderStateSettings;
    class CommandQueue;
    class Buffer;
    class BufferSet;
    class BufferSettings;
    class Texture;
    class TextureSettings;
    class Shader;
    enum class ShaderType;
    class ShaderSettings;
    class Program;
    class ProgramSettings;

    class Context: public Object {
    public:
        virtual Ref<CommandQueue> CreateCommandQueue() = 0;
        virtual Ref<Buffer> CreateBuffer(const BufferSettings& settings) = 0;
        virtual Ref<BufferSet> CreateBufferSet(const std::vector<Ref<Buffer>> buffers) = 0;
        virtual Ref<Texture> CreateTexture(const TextureSettings& settings) = 0;
        virtual Ref<Shader> CreateShader(ShaderType type, const ShaderSettings& settings) = 0;
        virtual Ref<Program> CreateProgram(const ProgramSettings& settings) = 0;
    protected:
    private:
    };

    class RenderContextSettings {
    public:
        RenderContextSettings() = default;
        RenderContextSettings(uint32_t width,
                              uint32_t height,
                              uint32_t frameBufferCount = 2,
                              PixelFormat colorFormat = PixelFormat::RGBA8Unorm_sRGB,
                              PixelFormat depthStencilFormat = PixelFormat::Depth32Float_Stencil8
                              ):
        colorFormat(colorFormat), depthStencilFormat(depthStencilFormat), width(width),
        height(height), frameBufferCount(frameBufferCount) { }
        
        const PixelFormat& GetColorFormat() const { return colorFormat; }
        const PixelFormat& GetDepthStencilFormat() const { return depthStencilFormat; }
        const uint32_t& GetWidth() const { return width; }
        const uint32_t& GetHeight() const { return height; }
        const uint32_t& GetFrameBufferCount() const { return frameBufferCount; }
    protected:
    private:
        PixelFormat colorFormat = PixelFormat::RGBA8Unorm_sRGB;
        PixelFormat depthStencilFormat = PixelFormat::Depth32Float_Stencil8;
        uint32_t width;
        uint32_t height;
        uint32_t frameBufferCount = 2;
    };

    class RenderContext: public Context {
    public:
        RenderContext(const RenderContextSettings& settings): settings(settings) {}
        
        const RenderContextSettings& GetSettings() const { return settings; }
        void SetSettings(const RenderContextSettings& _settings) { settings = _settings; }
        
        virtual Ref<RenderPattern> CreateRenderPattern(const RenderPatternSettings& settings) = 0;
        virtual Ref<RenderState> CreateRenderState(const RenderStateSettings& settings) = 0;
    protected:
    private:
        RenderContextSettings settings;
    };
}
