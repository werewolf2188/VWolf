//
//  HLSLOpenGLShader.hpp
//  VWolf
//
//  Created by Enrique Moises on 4/13/26.
//

#pragma once

#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Platform/Common/PShader.h"

#include "VWolf/Platform/OpenGL/Core/Bindable.h"

#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLDriver.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLShader.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLProgram.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderState.h"

namespace VWolf {

    class HLSLOpenGLShader: public PShader, public Bindable {
    public:
        // TODO: Compute shader is the only one that is different.
        HLSLOpenGLShader(Shader& coreShader);
        virtual ~HLSLOpenGLShader();

        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const override;
        virtual size_t GetMaterialSize() const override;
        virtual std::vector<ShaderInput> GetTextureInputs() const override;
        virtual std::vector<AttributeDescriptor> GetAttributes() const override;

        virtual std::string GetName() const override;
        
        const std::vector<Ref<RHI::OpenGLShader>> GetShaders () { return shaders; }
        const Ref<RHI::OpenGLProgram> GetProgram() { return program; }
        const Ref<RHI::OpenGLRenderState> GetState() { return state; }
    private:
        void CompileShaders(Shader& coreShader);
        void CompileProgram();
        void CompileRenderState(Shader& coreShader);
    private:
        std::vector<Ref<RHI::OpenGLShader>> shaders;
        Ref<RHI::OpenGLProgram> program;
        Ref<RHI::OpenGLRenderState> state;

        bool loadFromNewShader = false;
        Settings settings;
    };
}
