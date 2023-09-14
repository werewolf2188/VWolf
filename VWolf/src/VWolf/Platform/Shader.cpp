#include "vwpch.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Application.h"

#include "VWolf/Platform/OpenGL/Render/GLSLShader.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/DirectX12/Render/HLSLShader.h"
#endif

namespace VWolf {

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
                shader = CreateRef<GLSLShader>(name,
                                               otherShaders,
                                               configuration);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                shader = CreateRef<HLSLShader>(name,
                                               otherShaders,
                                               configuration);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Shader: Not yet implemented");
        }

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
