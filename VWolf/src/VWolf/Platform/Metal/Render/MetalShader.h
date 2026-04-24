//
//  MetalShader.h
//  VWolf
//
//  Created by Enrique Moises on 4/1/26.
//

#pragma once
#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Render/Material.h"
#include "VWolf/Platform/PShader.h"
#include "VWolf/Platform/Metal/Render/MetalBufferGroup.h"
#include "VWolf/Platform/Metal/Render/MetalTexture.h"

namespace VWolf {

    class MetalShader: public PShader {
    public:
    public:
        MetalShader(std::string name,
                    std::initializer_list<ShaderSource> otherShaders,
                    ShaderConfiguration configuration = {}): PShader(name, otherShaders, configuration) {}
        MetalShader(Shader& coreShader): PShader(coreShader) {};
        virtual ~MetalShader() {}
    public:
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual std::string GetName() const = 0;
        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) = 0;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const = 0;
        virtual size_t GetMaterialSize() const = 0;
        virtual std::vector<ShaderInput> GetTextureInputs() const = 0;
    public:
        virtual MTL::RenderPipelineState* GetState() = 0;
        virtual void SetObjectIndex(uint32_t index) = 0;
        virtual void SetVertexBufferIndex(Ref<MetalVertexBuffer> buffer) = 0;
        virtual void UseShader(MTL::RenderCommandEncoder* encoder) = 0;
        virtual void SetTextures(Ref<MetalRenderTexture> shadowMap, Material& material) = 0;
        virtual void Draw(MTL::PrimitiveType type, Ref<MetalIndexBuffer> buffer) = 0;
    };
}
