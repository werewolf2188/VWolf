//
//  Texture.cpp
//  VWolf
//
//  Created by Enrique Moises on 6/26/26.
//

#include "vwpch.h"
#include "Texture.h"

#include "VWolf/Core/Color.h"
#include "VWolf/Core/Math/VMath.h"

#include "stb_image/stb_image.h"

namespace VWolf {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(TextureWrapMode, None, Repeat, Clamp, Mirror, MirrorOnce);
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(TextureFilterMode, Point, Bilinear, Trilinear);
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER(TextureOptions);

    extern Ref<PTexture2D> LoadTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options);
    extern void* GetHandlerForTexture2D(Ref<PTexture2D> texture2d);

    extern Ref<PRenderTexture> LoadRenderTexture(uint32_t width, uint32_t height, TextureOptions options);
    extern void* GetHandlerForRenderTexture(Ref<PRenderTexture> renderTexture);
    extern void ResizeRenderTexture(Ref<PRenderTexture> renderTexture, uint32_t width, uint32_t height);

    extern Ref<PCubemap> LoadCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options);
    extern void* GetHandlerForRenderTexture(Ref<PCubemap> cubemap);

    namespace {
        // Define the 6 cubemap faces
        enum class CubemapFace {
            FACE_RIGHT = 0,  // +X
            FACE_LEFT = 1,   // -X
            FACE_TOP = 2,    // +Y
            FACE_BOTTOM = 3, // -Y
            FACE_FRONT = 4,  // +Z
            FACE_BACK = 5    // -Z
        };

        Color Transform(TextureDefault textureDefault) {
            switch(textureDefault) {
                case TextureDefault::White: return Color(1, 1, 1, 1);
                case TextureDefault::Bump: return Color(0.5f, 0.5f, 1, 0.5f);
                case TextureDefault::Black: return Color(0, 0, 0, 1);
                case TextureDefault::Gray: return Color(0.5f, 0.5f, 0.5f, 1);
                case TextureDefault::Red: return Color(1, 0, 0, 1);
            }
        }

        Color* CreateCheckerboardPattern(uint32_t width, uint32_t height) {
            size_t size = sizeof(Color) * width * height;
            Color* data = (Color*)malloc(size);
            memset(data, 0, size);
            uint32_t index = 0;
            Color black(0, 0, 0, 1);
            Color white(1, 1, 1, 1);
            Color value = white;
            for (uint32_t column = 0; column < height; column++) {
                if (column % 32 == 0) {
                    if (value.GetR() == 1)
                        value = black;
                    else if (value.GetR() == 0)
                        value = white;
                }
                for (uint32_t row = 0; row < width; row++) {
                    if (row % 32 == 0) {
                        if (value.GetR() == 1)
                            value = black;
                        else if (value.GetR() == 0)
                            value = white;
                    }
                    index = (column * height) + row;
                    data[index] = value;
                }
            }
            return data;
        }

        Color* CreateColorPattern(TextureDefault textureDefault, uint32_t width, uint32_t height) {
            size_t size = sizeof(Color) * width * height;
            Color* data = (Color*)malloc(size);
            memset(data, 0, size);
            uint32_t index = 0;
            Color value = Transform(textureDefault);
            for (uint32_t column = 0; column < height; column++) {
                for (uint32_t row = 0; row < width; row++) {
                    index = (column * height) + row;
                    data[index] = value;
                }
            }
            
            return data;
        }
    
        Color* CreateColoredCheckerboardPattern(uint32_t m_size, int checkIndex, Color otherColor) {
            size_t size = sizeof(Color) * m_size * m_size;
            Color* data = (Color*)malloc(size);
            memset(data, 0, size);
            uint32_t index = 0;
            Color white(1, 1, 1, 1);
            Color value = white;
            for (uint32_t column = 0; column < m_size; column++) {
                if (column % 32 == 0) {
                    if (value[checkIndex] == 1)
                        value = otherColor;
                    else if (value[checkIndex] == 0)
                        value = white;
                }
                for (uint32_t row = 0; row < m_size; row++) {
                    if (row % 32 == 0) {
                        if (value[checkIndex] == 1)
                            value = otherColor;
                        else if (value[checkIndex] == 0)
                            value = white;
                    }
                    index = (column * m_size) + row;
                    data[index] = value;
                }
            }
            return data;
        }
    
        std::array<void*, 6> CreateCubemapColorPattern(uint32_t m_size) {
            std::array<Color, 6> colors = {
                Color(1, 0, 0, 1),
                Color(0, 1, 0, 1),
                Color(0, 0, 1, 1),
                Color(1, 1, 0, 1),
                Color(1, 0, 1, 1),
                Color(0, 1, 1, 1)
            };

            std::array<int, 6> indicesToCheck = {
                1,
                2,
                0,
                2,
                1,
                0
            };
            std::array<void*, 6> m_data;
            for (unsigned int i = 0; i < 6; i++)
            {
                m_data[i] = CreateColoredCheckerboardPattern(m_size, indicesToCheck[i], colors[i]);
            }
            return m_data;
        }
    
        Vector3 GetCubemapVector(CubemapFace face, Vector2 texturePoint, uint32_t faceSize) {
            Vector2 uv = (texturePoint + 0.5f) / faceSize * 2.0f - 1.0f;
            
            Vector3 xyz;
            switch (face) {
                case CubemapFace::FACE_RIGHT:
                    xyz.SetX(1.0f);
                    xyz.SetY(-uv.GetY());
                    xyz.SetZ(-uv.GetX());
                    break;
                case CubemapFace::FACE_LEFT:
                    xyz.SetX(-1.0f);
                    xyz.SetY(-uv.GetY());
                    xyz.SetZ(uv.GetX());
                    break;
                case CubemapFace::FACE_TOP:
                    xyz.SetX(uv.GetX());
                    xyz.SetY(1.0f);
                    xyz.SetZ(uv.GetY());
                    break;
                case CubemapFace::FACE_BOTTOM:
                    xyz.SetX(uv.GetX());
                    xyz.SetY(-1.0f);
                    xyz.SetZ(-uv.GetY());
                    break;
                case CubemapFace::FACE_FRONT:
                    xyz.SetX(uv.GetX());
                    xyz.SetY(-uv.GetY());
                    xyz.SetZ(1.0f);
                    break;
                case CubemapFace::FACE_BACK:
                    xyz.SetX(-uv.GetX());
                    xyz.SetY(-uv.GetY());
                    xyz.SetZ(-1.0f);
                    break;
            }
            
            xyz.Normalize();
            
            return xyz;
        }

        Vector4 samplePanorama(const float* panoData, int panoWidth, int panoHeight, Vector3 cartesianCoord) {
            float theta = std::atan2(cartesianCoord.GetZ(), cartesianCoord.GetX());
            float phi = std::asin(cartesianCoord.GetY());
            
            float u = (theta + M_PI) / (2.0f * M_PI);
            float v = (phi + M_PI / 2.0f) / M_PI;
            
            int x = std::min(static_cast<int>(u * panoWidth), panoWidth - 1);
            int y = std::min(static_cast<int>(v * panoHeight), panoHeight - 1);
            
            int index = (y * panoWidth + x) * 3;
            
            return Vector4(panoData[index], panoData[index + 1], panoData[index + 2], 1);
        }
    }

    Texture2D::Texture2D(UUID _id, TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options): m_textureDefault(textureDefault), m_width(width), m_height(height), Texture(_id, options) {
        Color* colors = CreateColorPattern(textureDefault, width, height);
        _innerTexture = LoadTexture2D(colors, width, height, options);
    }

    Texture2D::Texture2D(UUID _id, const std::filesystem::path filePath, TextureOptions options): Texture(_id, options) {
        int channels, width, height;
        auto img = stbi_loadf(filePath.string().c_str(), &width, &height, &channels, 0);
        name = filePath.stem().string();
        m_width = width;
        m_height = height;
        _innerTexture = LoadTexture2D(img, width, height, options);
    }

    void* Texture2D::GetHandler() {
        return GetHandlerForTexture2D(_innerTexture);
    }

    Ref<Texture2D> Texture2D::Load(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options) {
        Ref<Texture2D> tex = CreateRef<Texture2D>(UUID::Empty, textureDefault, width, height, options);
        return tex;
    }

    Ref<Texture2D> Texture2D::Load(UUID _id, const std::filesystem::path filePath, TextureOptions options) {
        Ref<Texture2D> tex = CreateRef<Texture2D>(_id, filePath, options);
        ObjectResourceManager::AddObject(_id, tex);
        return tex;
    }

    RenderTexture::RenderTexture(uint32_t width, uint32_t height, TextureOptions options): m_width(width), m_height(height), Texture(UUID::NewUUID(), options)  {
        _innerTexture = LoadRenderTexture(width, height, options);
    }

    void* RenderTexture::GetHandler() {
        return GetHandlerForRenderTexture(_innerTexture);
    }

    void RenderTexture::Resize(uint32_t width, uint32_t height) {
        m_width = width;
        m_height = height;
        ResizeRenderTexture(_innerTexture, width, height);
    }

    Cubemap::Cubemap(UUID _id, TextureDefault textureDefault, uint32_t size, TextureOptions options): m_textureDefault(textureDefault), m_size(size), Texture(_id, options) {
        std::array<void*, 6> colors = CreateCubemapColorPattern(size);
        _innerTexture = LoadCubemap(colors, size, options);
    }

    Cubemap::Cubemap(UUID _id, std::filesystem::path path, TextureOptions options): Texture(_id, options) {
        std::array<void*, 6> colors;
        int channels, width, height;
        name = path.stem().string();
        stbi_set_flip_vertically_on_load(true);
        float* img = stbi_loadf(path.string().c_str(), &width, &height, &channels, 3);
        
        m_size = height / 2;
        for (int f = 0; f < 6; ++f) {
            
            size_t size = sizeof(Vector4) * m_size * m_size;
            Vector4* data = (Vector4*)malloc(size);
            memset(data, 0, size);
            uint32_t index = 0;
            
            for (int y = 0; y < m_size; ++y) {
                for (int x = 0; x < m_size; ++x) {
                    CubemapFace face = (CubemapFace)f;
                    Vector3 coord = GetCubemapVector(face, Vector2(x, y), m_size);
                    Vector4 pixel = samplePanorama(img, width, height, coord);
                    
                    index = (y * m_size) + x;
                    data[index] = pixel;
                }
            }
            
            colors[f] = data;
        }
        
        _innerTexture = LoadCubemap(colors, m_size, options);
    }

    void* Cubemap::GetHandler() {
        return GetHandlerForRenderTexture(_innerTexture);
    }

    Ref<Cubemap> Cubemap::Load(TextureDefault textureDefault, uint32_t size, TextureOptions options) {
        Ref<Cubemap> tex = CreateRef<Cubemap>(UUID::Empty, textureDefault, size, options);
        return tex;
    }

    Ref<Cubemap> Cubemap::Load(UUID _id, std::filesystem::path path, TextureOptions options) {
        Ref<Cubemap> tex = CreateRef<Cubemap>(_id, path, options);
        ObjectResourceManager::AddObject(_id, tex);
        return tex;
    }
}
