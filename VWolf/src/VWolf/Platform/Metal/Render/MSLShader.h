//
//  MSLShader.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/25/24.
//

#pragma once

#include "VWolf/Core/Render/Shader.h"

#include "VWolf/Platform/Metal/Core/Core.h"

namespace VWolf {
    class MLProgram;

    class MSLShader: public Shader {
    public:
        MSLShader(std::string name,
                   std::initializer_list<ShaderSource> otherShaders,
                   ShaderConfiguration configuration = {});
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
        MTL::RenderPipelineState* GetState();
        NS::UInteger GetIndex(const char* name);
        NS::UInteger GetVertexBufferIndex();
        void UseShader(MTL::RenderCommandEncoder* encoder);
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
