//
//  HLSLToMetal.hpp
//  VWolf
//
//  Created by Enrique Moises on 3/28/26.
//

#pragma once

#include "VWolf/Platform/Metal/Render/MetalShader.h"

namespace VWolf {
    class HLMetalProgram;
//    std::map<std::string, MTL::Library*> CompileHLSLLibraries(std::initializer_list<ShaderSource> otherShaders);

    class HLSLMetalShader: public MetalShader {
    public:
        HLSLMetalShader(std::string name,
                        std::initializer_list<ShaderSource> otherShaders,
                        ShaderConfiguration configuration = {});
        HLSLMetalShader(Shader& coreShader);
        virtual ~HLSLMetalShader();
    public:
        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual std::string GetName() const override;
        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) override;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const override;
        virtual size_t GetMaterialSize() const override;
        virtual std::vector<ShaderInput> GetTextureInputs() const override;
    public:
        virtual MTL::RenderPipelineState* GetState() override;
        virtual void SetObjectIndex(uint32_t index)override;
        virtual void SetVertexBufferIndex(Ref<MetalVertexBuffer> buffer) override;
        virtual void UseShader(MTL::RenderCommandEncoder* encoder) override;
        virtual void SetTextures(Ref<MetalRenderTexture> shadowMap, Material& material) override;
        virtual void Draw(MTL::PrimitiveType type, Ref<MetalIndexBuffer> buffer) override;
    private:
        void SetConfiguration() const;
        void SetRasterization() const;
        void SetBlend() const;
        void SetDepthStencil() const;
    private:
        uint32_t currentIndex = 0;
        Ref<HLMetalProgram> hlMetalProgram;
        MTL::RenderCommandEncoder* encoder = nullptr;
        bool loadFromNewShader = false;
        Settings settings;
    };
}
