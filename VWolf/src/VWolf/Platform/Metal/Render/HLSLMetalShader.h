//
//  HLSLToMetal.hpp
//  VWolf
//
//  Created by Enrique Moises on 3/28/26.
//

#pragma once

#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Render/Material.h"
#include "VWolf/Platform/Common/PShader.h"
#include "VWolf/Platform/Metal/Render/MetalTexture.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalBuffer.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalShader.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalProgram.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderState.h"

namespace VWolf {
    class HLSLMetalShader: public PShader {
    public:
        HLSLMetalShader(Shader& coreShader);
        virtual ~HLSLMetalShader();
    public:

        virtual std::string GetName() const override;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const override;
        virtual size_t GetMaterialSize() const override;
        virtual std::vector<ShaderInput> GetTextureInputs() const override;
        virtual std::vector<AttributeDescriptor> GetAttributes() const override;
    public:
        const Ref<RHI::MetalProgram> GetProgram() const { return program; }
        const Ref<RHI::MetalRenderState> GetState() const { return state; }
    private:
        Settings settings;
        
        std::vector<Ref<RHI::MetalShader>> shaders;
        Ref<RHI::MetalProgram> program;
        Ref<RHI::MetalRenderState> state;
        
        void CompileShaders(Shader& coreShader);
        void CompileProgram();
        void BuildState(Shader& coreShader);
    };
}
