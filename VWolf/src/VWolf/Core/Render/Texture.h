//
//  Texture.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#pragma once
#include "VWolf/Core/Base.h"
#include "VWolf/Core/Object.h"

#include "VWolf/Core/Utils/GenericSerialization.h"
namespace VWolf {
    class Texture2D;
    class RenderTexture;
    class Cubemap;

    enum class TextureDefault {
        White, Gray, Black, Bump, Red
    };

    enum class TextureWrapMode {
        None, Repeat, Clamp, Mirror, MirrorOnce
    };

    BOOST_DESCRIBE_ENUM(TextureWrapMode, None, Repeat, Clamp, Mirror, MirrorOnce)

    enum class TextureFilterMode {
        Point, Bilinear, Trilinear
    };

    BOOST_DESCRIBE_ENUM(TextureFilterMode, Point, Bilinear, Trilinear)

    class TextureOptions {
    public:
        TextureFilterMode GetFilterMode() const { return filterMode; }
        TextureWrapMode GetWrapModeU() const { return wrapModeU; }
        TextureWrapMode GetWrapModeV() const { return wrapModeV; }
        TextureWrapMode GetWrapModeW() const { return wrapModeW; }
        TextureWrapMode GetWrapMode() const { return wrapMode; }
    private:
        TextureFilterMode filterMode = TextureFilterMode::Bilinear;
        TextureWrapMode wrapModeU = TextureWrapMode::None;
        TextureWrapMode wrapModeV = TextureWrapMode::None;
        TextureWrapMode wrapModeW = TextureWrapMode::None;
        TextureWrapMode wrapMode = TextureWrapMode::Repeat;
        
        BOOST_DESCRIBE_CLASS(TextureOptions, (), (), (), (filterMode, wrapModeU, wrapModeV, wrapModeW, wrapMode))
        VWOLF_SERIALIZATION_FRIENDS(TextureOptions)
    };

    class Texture: public Object {
    public:
        Texture(UUID _id, TextureOptions options = {}): Object(_id), m_options(options) {}
        virtual ~Texture() = default;
        virtual void* GetHandler() = 0;
    public:
        TextureOptions GetOptions() { return m_options; }
    private:
        TextureOptions m_options;
    };

    class PTexture2D;

    class Texture2D: public Texture {
    public:
        Texture2D(UUID _id, TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options = {});
        Texture2D(UUID _id, const std::filesystem::path filePath, TextureOptions options = {});
        ~Texture2D() = default;
    public:
        uint32_t GetWidth() { return m_width; }
        uint32_t GetHeight() { return m_height; }
        Ref<PTexture2D> GetInnerTexture() { return _innerTexture; }
    public:
        virtual void* GetHandler() override;
    public:
        static Ref<Texture2D> Load(TextureDefault textureDefault = TextureDefault::White, uint32_t width = 512, uint32_t height = 512, TextureOptions options = {});
        static Ref<Texture2D> Load(UUID _id, const std::filesystem::path filePath, TextureOptions options = {});
    private:
        Ref<PTexture2D> _innerTexture;
        
        uint32_t m_width;
        uint32_t m_height;
        TextureDefault m_textureDefault;
    };

    class PRenderTexture;

    class RenderTexture: public Texture {
    public:
        RenderTexture(uint32_t width, uint32_t height, TextureOptions options = {});
        ~RenderTexture() = default;
    public:
        uint32_t GetWidth() { return m_width; }
        uint32_t GetHeight() { return m_height; }
        Ref<PRenderTexture> GetInnerTexture() { return _innerTexture; }
    public:
        virtual void* GetHandler() override;
    public:
        void Resize(uint32_t width, uint32_t height);
    private:
        uint32_t m_width;
        uint32_t m_height;
        
        Ref<PRenderTexture> _innerTexture;
    };

    class PCubemap;

    class Cubemap: public Texture {
    public:
        Cubemap(UUID _id, TextureDefault textureDefault, uint32_t size, TextureOptions options = {});
        Cubemap(UUID _id, std::filesystem::path path, TextureOptions options = {});
        ~Cubemap() = default;
    public:
        uint32_t GetSize() { return m_size; }
        Ref<PCubemap> GetInnerTexture() { return _innerTexture; }
    public:
        virtual void* GetHandler() override;
    public:
        static Ref<Cubemap> Load(TextureDefault textureDefault = TextureDefault::White, uint32_t size = 512, TextureOptions options = {});
        static Ref<Cubemap> Load(UUID _id, std::filesystem::path path, TextureOptions options = {});
    private:
        uint32_t m_size;
        TextureDefault m_textureDefault;
        
        Ref<PCubemap> _innerTexture;
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::TextureWrapMode, None, Repeat, Clamp, Mirror, MirrorOnce)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::TextureFilterMode, Point, Bilinear, Trilinear)

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::TextureOptions)
}
