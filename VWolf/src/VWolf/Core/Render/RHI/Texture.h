//
//  Texture.h
//  VWolf
//
//  Created by Enrique Moises on 8/26/26.
//

#pragma once

#include "Types.h"
#include "Context.h"
#include "Resource.h"

namespace VWolf::RHI {

    enum class TextureType {
        Image,
        RenderTarget,
        FrameBuffer,
        DepthStencil
    };

    enum class TextureDimensionType
    {
        Tex1D,
        Tex1DArray,
        Tex2D,
        Tex2DArray,
        Tex2DMultisample,
        Cube,
        CubeArray,
        Tex3D,
    };

    class TextureSettings {
    public:
        TextureSettings() = default;
        static TextureSettings ForImage(const PixelFormat format,
                                        const uint32_t width,
                                        const uint32_t height = 1,
                                        const uint32_t depth = 1,
                                        const uint32_t arrayLength = 1,
                                        const bool mipmapped = false,
                                        ResourceUsage usage = ResourceUsage::ShaderRead) {
            
            TextureType tType = TextureType::Image;
            TextureDimensionType tdType = TextureDimensionType::Tex1D;
            
            if (arrayLength > 1) {
                if (height > 1) {
                    tdType = TextureDimensionType::Tex2DArray;
                } else {
                    tdType = TextureDimensionType::Tex1DArray;
                }
            } else {
                if (depth > 1 && height > 1) {
                    tdType = TextureDimensionType::Tex3D;
                } else if (height > 1) {
                    tdType = TextureDimensionType::Tex2D;
                } else {
                    tdType = TextureDimensionType::Tex1D;
                }
            }
            
            return {
                tType,
                tdType,
                format,
                width,
                height,
                depth,
                arrayLength,
                mipmapped,
                usage
            };
        }
        
        static TextureSettings ForCubeImage(const PixelFormat format,
                                            const uint32_t size,
                                            const uint32_t arrayLength = 1,
                                            const bool mipmapped = false,
                                            ResourceUsage usage = ResourceUsage::ShaderRead) {
            TextureType tType = TextureType::Image;
            TextureDimensionType tdType = TextureDimensionType::Cube;
            
            if (arrayLength > 1) {
                tdType = TextureDimensionType::CubeArray;
            } else {
                tdType = TextureDimensionType::Cube;
            }
            
            return {
                tType,
                tdType,
                format,
                size,
                size,
                1,
                arrayLength,
                mipmapped,
                usage
            };
        }
        
        static TextureSettings ForFrameBuffer(const PixelFormat format,
                                              const uint32_t width,
                                              const uint32_t height = 1,
                                              const bool mipmapped = false,
                                              ResourceUsage usage = ResourceUsage::ShaderRead | ResourceUsage::RenderTarget) {
            TextureType tType = TextureType::RenderTarget;
            TextureDimensionType tdType = TextureDimensionType::Tex2D;
            
            return {
                tType,
                tdType,
                format,
                width,
                height,
                1,
                1,
                mipmapped,
                usage
            };
        }
        
        static TextureSettings ForFrameBuffer(const RenderContextSettings& settings) {
            TextureType tType = TextureType::FrameBuffer;
            TextureDimensionType tdType = TextureDimensionType::Tex2D;
            
            return {
                tType,
                tdType,
                settings.GetColorFormat(),
                settings.GetWidth(),
                settings.GetHeight(),
                1,
                1,
                false
            };
        }
        
        static TextureSettings ForDepthStencil(const PixelFormat format,
                                               const uint32_t width,
                                               const uint32_t height = 1,
                                               const bool mipmapped = false,
                                               ResourceUsage usage = ResourceUsage::ShaderRead | ResourceUsage::RenderTarget) {
            TextureType tType = TextureType::DepthStencil;
            TextureDimensionType tdType = TextureDimensionType::Tex2D;
            
            return {
                tType,
                tdType,
                format,
                width,
                height,
                1,
                1,
                mipmapped,
                usage
            };
        }
        
        static TextureSettings ForDepthStencil(const RenderContextSettings& settings) {
            TextureType tType = TextureType::DepthStencil;
            TextureDimensionType tdType = TextureDimensionType::Tex2D;
            
            return {
                tType,
                tdType,
                settings.GetDepthStencilFormat(),
                settings.GetWidth(),
                settings.GetHeight(),
                1,
                1,
                false,
                ResourceUsage::ShaderRead | ResourceUsage::RenderTarget
            };
        }
        
        const TextureType& GetTextureType() const { return textureType; }
        const TextureDimensionType& GetTextureDimensionType() const { return textureDimensionType; }
        const PixelFormat& GetFormat() const { return format; }
        const uint32_t& GetWidth() const { return width; }
        const uint32_t& GetHeight() const { return height; }
        const uint32_t& GetDepth() const { return depth; }
        const uint32_t& GetArrayLength() const { return arrayLength; }
        const bool& GetMipmapped() const { return mipmapped; }
        const ResourceUsage& GetResourceUsage() const { return usage; }
    protected:
    private:
        TextureSettings(
                        TextureType textureType,
                        TextureDimensionType textureDimensionType,
                        PixelFormat format,
                        uint32_t width,
                        uint32_t height,
                        uint32_t depth,
                        uint32_t arrayLength,
                        bool mipmapped,
                        ResourceUsage usage = ResourceUsage::ShaderRead
                        ):
        textureType(textureType), textureDimensionType(textureDimensionType), format(format), width(width),
        height(height), depth(depth), arrayLength(arrayLength), mipmapped(mipmapped), usage(usage) {}
        
        TextureType textureType;
        TextureDimensionType textureDimensionType = TextureDimensionType::Tex2D;
        PixelFormat format;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t arrayLength = 1U;
        bool mipmapped = false;
        ResourceUsage usage;
    };

    class TextureView;

    class Texture: public Resource {
    public:
        Texture(const TextureSettings& settings): settings(settings) {}
        
        virtual const ResourceType GetResourceType() const override { return ResourceType::Texture; }
        
        virtual void SetData(const SubResource& subResource) = 0;
        virtual TextureView GetTextureView(const SubResourceIndex& index) = 0;
        
        const TextureSettings& GetSettings() const { return settings; }
    protected:
        TextureSettings settings;
    private:
    };

    class TextureView {
    public:
        TextureView() = default;
        TextureView(Weak<Texture> texture, const SubResourceIndex& index): texture(texture), index(index) {}
        
        const Weak<Texture> GetTexture() const { return texture; }
        const SubResourceIndex& GetIndex() { return index; }
    private:
        Weak<Texture> texture;
        SubResourceIndex index;
    };
}
