//
//  PShader.h
//  VWolf
//
//  Created by Enrique Moises on 4/26/26.
//

#pragma once

#include "VWolf/Core/Render/Shader.h"

namespace VWolf {
    class PShader {
    public:
        PShader(Shader& coreShader): m_name(coreShader.name) {}
        
        virtual ~PShader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const = 0;
        virtual size_t GetMaterialSize() const = 0;
        virtual std::vector<ShaderInput> GetTextureInputs() const = 0;

        virtual std::string GetName() const = 0;

        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) = 0;
    protected:
        std::string m_name;
    };
}
