#include "vwpch.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Application.h"

#include "VWolf/Platform/OpenGL/Render/GLSLShader.h"
#include "VWolf/Platform/OpenGL/Render/HLSLOpenGLShader.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/DirectX12/Render/HLSLShader.h"
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "VWolf/Platform/Metal/Render/MSLShader.h"
#include "VWolf/Platform/Metal/Render/HLSLMetalShader.h"
#endif

namespace VWolf {

    enum class ProgramType {
        GLSL, MSL, HLSL, UNKNOWN
    };

    ProgramType getShaderExtension(std::initializer_list<ShaderSource> otherShaders) {
        if (otherShaders.size() == 0) return ProgramType::UNKNOWN;
        
        const std::filesystem::path path = otherShaders.begin()->shader;
        if (path.extension() == ".metal") { // OLD SHADER FILE
            return ProgramType::MSL;
        } else if (path.extension() == ".hlsl") { // NEW SHADER FILE
            return ProgramType::HLSL;
        } else if (path.extension() == ".glsl") { // OLD SHADER FILE
            return ProgramType::GLSL;
        }
        return ProgramType::UNKNOWN;
    }

    std::vector<Ref<Shader>> ShaderLibrary::m_shaders;
    std::map<ShaderLibrary::ShaderSpecialty, std::string> ShaderLibrary::m_specialtiesShaders;

    // TODO: Remove
    const char* ShaderLibrary::CameraBufferName = "Camera";

    const char* ShaderLibrary::ObjectBufferName = "Object";
    //

    void ShaderLibrary::LoadShader(std::string name,
                                   std::initializer_list<ShaderSource> otherShaders,
                                   ShaderConfiguration configuration) {
        Ref<Shader> shader;
        
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                switch (getShaderExtension(otherShaders)) {
                    case ProgramType::GLSL:
                        shader = CreateRef<GLSLShader>(name,
                                                       otherShaders,
                                                       configuration);
                        break;
                    case ProgramType::HLSL:
                        shader = CreateRef<HLSLOpenGLShader>(name,
                                                             otherShaders,
                                                             configuration);
                        break;
                    default:
                        VWOLF_CORE_ASSERT("This file is unsupported");
                }
                
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                shader = CreateRef<HLSLShader>(name,
                                               otherShaders,
                                               configuration);
                break;
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                
                switch (getShaderExtension(otherShaders)) {
                    case ProgramType::MSL:
                        shader = CreateRef<MSLShader>(name,
                                                      otherShaders,
                                                      configuration);
                        break;
                    case ProgramType::HLSL:
                        shader = CreateRef<HLSLMetalShader>(name,
                                                            otherShaders,
                                                            configuration);
                        break;
                    default:
                        VWOLF_CORE_ASSERT("This file is unsupported");
                }
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Shader: Not yet implemented");
        }

        if (shader != nullptr)
            m_shaders.push_back(shader);
    }

    Ref<Shader> ShaderLibrary::GetShader(std::string name) {
        for (auto shader: m_shaders) {
            std::string shaderName = shader->GetName();
            if (shaderName == name) {
                return shader;
            }
        }
        return nullptr;
    }

    Ref<Shader> ShaderLibrary::GetShader(ShaderSpecialty type) {
        return GetShader(m_specialtiesShaders[type]);
    }

    void ShaderLibrary::SetShaderSpecialty(std::string name, ShaderSpecialty type) {
        m_specialtiesShaders[type] = name;
    }
}
