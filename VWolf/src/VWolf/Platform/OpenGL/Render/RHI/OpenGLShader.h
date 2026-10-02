//
//  OpenGLShader.h
//  VWolf
//
//  Created by Enrique Moises on 10/8/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"

#include "VWolf/Core/Render/RHI/Shader.h"

namespace VWolf::DXIL {
    class Shader;
}

namespace VWolf::RHI {
    class OpenGLShader: public Shader {
    public:
        OpenGLShader(Weak<Context> context, ShaderType type, const ShaderSettings& settings);
        ~OpenGLShader();
        
        const bool HasBeenDeleted() const { return GetStatus(GL_DELETE_STATUS) == GL_TRUE; }
        const bool IsCompiled() const { return GetStatus(GL_COMPILE_STATUS) == GL_TRUE; }
        const GLint GetSize() const { return GetStatus(GL_SHADER_SOURCE_LENGTH); }
        
        const GLuint GetShaderID() const { return shaderId; }
        
        const bool IsAttached() const { return isAttached; }
        void Attach(GLuint programId);
        void Detach(GLuint programId);
        
        const Ref<VWolf::DXIL::Shader> GetDXILCompiledShader() const { return dxil; }
        const Ref<VWolf::DXIL::Shader> GetDXILCompiledShaderWithAttributes() const { return dxilWithAttributes; }
    private:
        void TransformCoreIntoDXIL();
        void TranslateFromDXILToGLSL();
        void Compile();
    private:
        const GLint GetStatus(GLenum enumValue) const;
        void PrintError();
        
        Ref<VWolf::DXIL::Shader> dxil, dxilWithAttributes;
        std::string sourceText;
        
        GLuint shaderId;
        GLenum glType;
        bool isAttached;
    };
}
