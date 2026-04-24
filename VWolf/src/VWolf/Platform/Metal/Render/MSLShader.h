//
//  MSLShader.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/25/24.
//

#pragma once

#include "VWolf/Platform/Metal/Render/MetalShader.h"

namespace VWolf {
    class MLProgram;

    class [[deprecated("This class is deprecated. Use HLSLMetalShader instead")]] MSLShader: public MetalShader {
    public:
        MSLShader(std::string name,
                   std::initializer_list<ShaderSource> otherShaders,
                   ShaderConfiguration configuration = {});
        MSLShader(Shader& coreShader);
        virtual ~MSLShader();
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
        virtual void SetObjectIndex(uint32_t index) override;
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
        Ref<MLProgram> mlProgram;
        MTL::RenderCommandEncoder* encoder = nullptr;
    };
}
