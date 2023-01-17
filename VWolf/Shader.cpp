#include "vwpch.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Application.h"

#include "VWolf/Platform/OpenGL/Render/GLSLShader.h"
// TODO: Will move
#include "VWolf/Platform/Render/HLSLShader.h"

namespace VWolf {

    std::vector<Ref<Shader>> ShaderLibrary::m_shaders;

    // TODO: Remove
    const char* ShaderLibrary::CameraBufferName = "Camera";

    const char* ShaderLibrary::ObjectBufferName = "Object";
    //

    void ShaderLibrary::LoadShader(const char* name,
                                   std::initializer_list<ShaderSource> otherShaders,
                                   ShaderConfiguration configuration) {
        Ref<Shader> shader;
        
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                shader = CreateRef<GLSLShader>(name,
                                               MeshData::Layout,
                                               otherShaders,
                                               configuration);
                break;
            default:
                VWOLF_CORE_ASSERT(false, "Shader: Not yet implemented");
        }

        m_shaders.push_back(shader);
    }

    Ref<Shader> ShaderLibrary::GetShader(const char* name) {
        for (auto shader: m_shaders) {
            std::string shaderName = shader->GetName();
            if (shaderName == name) {
                return shader;
            }
        }
        return nullptr;
    }
}