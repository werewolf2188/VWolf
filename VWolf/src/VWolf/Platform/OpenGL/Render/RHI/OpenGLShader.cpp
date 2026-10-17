//
//  OpenGLShader.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/8/26.
//

#include "vwpch.h"
#include "VWolf/Platform/DXILHelpers.h"
#include "OpenGLShader.h"

#include <spirv_glsl.hpp>

namespace VWolf::RHI {

    namespace {
        std::string SanitizeNameForVertexForInput(std::string name) {
            if (name.rfind("in.var.", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(7); // Remove "in"
                
                // 3. Rename the variable
                return new_name;
            }
            return name;
        }
    
        std::string SanitizeNameForVertexForOutputOrFragmentForInput(std::string name) {
            if (name.rfind("in.var.a", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(8); // Remove "in"
                
                // 3. Rename the variable
                return "v" + new_name;
            } if (name.rfind("in.var.", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(7); // Remove "in"
                
                // 3. Rename the variable
                return "v_" + new_name;
            } else if (name.rfind("out.var.a", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(9); // Remove "in"
                
                // 3. Rename the variable
                return "v" + new_name;
            } else if (name.rfind("out.var.", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(8); // Remove "in"
                
                // 3. Rename the variable
                return "v_" + new_name;
            }
            return name;
        }
    
        GLuint ShaderTypeEquivalent(VWolf::ShaderType type) {
            switch (type) {
                case VWolf::ShaderType::Vertex: return GL_VERTEX_SHADER;
                case VWolf::ShaderType::Fragment: return GL_FRAGMENT_SHADER;
                case VWolf::ShaderType::Pre_Tesselator: return GL_TESS_CONTROL_SHADER;
                case VWolf::ShaderType::Post_Tesselator: return GL_TESS_EVALUATION_SHADER;
                case VWolf::ShaderType::Geometry: return GL_GEOMETRY_SHADER;
                default:
                    VWOLF_CORE_ASSERT("Incorrect Shader Type");
            }
            return -1;
        }
    }

    OpenGLShader::OpenGLShader(Weak<Context> context, ShaderType type, const ShaderSettings& settings):
    Shader(context, type, settings) {
        TransformCoreIntoDXIL();
        TranslateFromDXILToGLSL();
        glType = ShaderTypeEquivalent((VWolf::ShaderType)(uint32_t)type);
        shaderId = glCreateShader(glType);
        GLThrowIfFailedNoAction("glCreateShader");
        Compile();
    }

    OpenGLShader::~OpenGLShader() {
        GLThrowIfFailed(glDeleteShader(shaderId));
    }

    void OpenGLShader::Attach(GLuint programId) {
        GLThrowIfFailed(glAttachShader(programId, shaderId));
        isAttached = true;
    }

    void OpenGLShader::Detach(GLuint programId) {
        GLThrowIfFailed(glDetachShader(programId, shaderId));
        isAttached = false;
    }

    void OpenGLShader::TransformCoreIntoDXIL() {
        dxil = CreateRef<VWolf::DXIL::Shader>(VWolf::DXIL::Shader(settings.GetShaderName(),
                                                                  settings.GetEntryFunction().GetFuncionName(),
                                                                  (VWolf::ShaderType)(uint32_t)type,
                                                                  settings.GetCode(),
                                                                  DXIL::Shader::ArgumentType::OpenGL));
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        dxilWithAttributes = CreateRef<VWolf::DXIL::Shader>(VWolf::DXIL::Shader(settings.GetShaderName(),
                                                                                settings.GetEntryFunction().GetFuncionName(),
                                                                                (VWolf::ShaderType)(uint32_t)type,
                                                                                settings.GetCode(),
                                                                                DXIL::Shader::ArgumentType::Metal));
#endif
#ifdef VWOLF_PLATFORM_WINDOWS
        dxilWithAttributes = CreateRef<VWolf::DXIL::Shader>(VWolf::DXIL::Shader(settings.GetShaderName(),
                                                                                settings.GetEntryFunction().GetFuncionName(),
                                                                                (VWolf::ShaderType)(uint32_t)type,
                                                                                settings.GetCode(),
                                                                                DXIL::Shader::ArgumentType::DirectX));
#endif
    }

    void OpenGLShader::TranslateFromDXILToGLSL() {
        SmartPoint<IDxcBlob>& blob = dxil->GetShader();
        uint32_t* pointer = reinterpret_cast<uint32_t*>(blob->GetBufferPointer());
        size_t di = blob->GetBufferSize() / sizeof(uint32_t);
        std::vector<uint32_t> spirv(pointer, pointer + di);
        spirv_cross::CompilerGLSL glsl(std::move(spirv));
        glsl.build_combined_image_samplers();
        // The SPIR-V is now parsed, and we can perform reflection on it.
        spirv_cross::ShaderResources resources = glsl.get_shader_resources();

        // Get all sampled images in the shader.
        for (auto &resource : resources.stage_inputs)
        {
            if (dxil->GetType() == VWolf::ShaderType::Vertex) {
                std::string new_name = SanitizeNameForVertexForInput(resource.name);
                glsl.set_name(resource.id, new_name);
            } else if (dxil->GetType() == VWolf::ShaderType::Fragment) {
                std::string new_name = SanitizeNameForVertexForOutputOrFragmentForInput(resource.name);
                glsl.set_name(resource.id, new_name);
            }

        }
        
        for (auto &resource : resources.stage_outputs)
        {
            if (dxil->GetType() == VWolf::ShaderType::Vertex) {
                std::string new_name = SanitizeNameForVertexForOutputOrFragmentForInput(resource.name);
                glsl.set_name(resource.id, new_name);
            }
        }

        uint32_t index = 0;
        for (auto &resource : resources.sampled_images)
        {
            auto& texture = resources.separate_images[index];
            std::string name = texture.name;
            std::cout << name << std::endl;
            glsl.set_name(resource.id, name);
            index++;
        }

        // Set some options.
        spirv_cross::CompilerGLSL::Options options;
        options.version = OPENGL_VERSION;
        options.enable_420pack_extension = false;
        glsl.set_common_options(options);
        

        // Compile to GLSL, ready to give to GL driver.
        sourceText = glsl.compile();
    }

    void OpenGLShader::Compile() {
        const char* shaderText = sourceText.c_str();
        GLThrowIfFailed(glShaderSource(shaderId, 1, &shaderText, NULL));
        GLThrowIfFailed(glCompileShader(shaderId));
        if (!IsCompiled()) {
            PrintError();
            VWOLF_CORE_ASSERT(false, "Something happened");
        }
    }

    const GLint OpenGLShader::GetStatus(GLenum enumValue) const {
        GLint success;
        GLThrowIfFailed(glGetShaderiv(shaderId, enumValue, &success));
        return success;
    }

    void OpenGLShader::PrintError() {
        GLint length = GetStatus(GL_INFO_LOG_LENGTH);
        GLchar* infoLog = new GLchar[length];
        GLThrowIfFailed(glGetShaderInfoLog(shaderId, length, nullptr, infoLog));
        VWOLF_CORE_DEBUG(infoLog);
        delete[] infoLog;
    }
}
