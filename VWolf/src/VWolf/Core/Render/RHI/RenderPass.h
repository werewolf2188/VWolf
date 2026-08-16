//
//  RenderPass.h
//  VWolf
//
//  Created by Enrique Moises on 8/17/26.
//

#pragma once

#include "RenderPattern.h"
#include "Texture.h"
#include "Object.h"

namespace VWolf::RHI  {

    class RenderPassSettings {
    public:
        RenderPassSettings() = default;
        RenderPassSettings(std::vector<TextureView> textures, uint32_t width, uint32_t height): textures(textures), width(width), height(height) {}
        
        const uint32_t GetWidth() const { return width; }
        const uint32_t GetHeight() const { return height; }
        
        const std::vector<TextureView>& GetTextures() const { return textures; }
        const void SetWidth(uint32_t _width) { width = _width; }
        const void SetHeight(uint32_t _height) { height = _height; }
    private:
        std::vector<TextureView> textures;
        uint32_t width;
        uint32_t height;
    };

    class RenderPass: public Object {
    public:
        RenderPass(const RenderPattern& pattern, const RenderPassSettings& settings): pattern(pattern), settings(settings) {}
        
        virtual void Resize(uint32_t width, uint32_t height) = 0;
    
        const RenderPattern& GetPattern() { return pattern; }
        const RenderPassSettings& GetSettings() { return settings; }
    protected:
        const RenderPattern& pattern;
        RenderPassSettings settings;
    private:
    };
}
