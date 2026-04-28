//
//  Shader.cpp
//  VWolf
//
//  Created by Enrique Moises on 4/23/26.
//

#include "vwpch.h"
#include "Shader.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::ShaderType, Vertex, Pre_Tesselator, Post_Tesselator, Geometry, Fragment)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::FillMode, Wireframe, Solid);
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::CullMode, Back, Front, FrontAndBack)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::DepthFunction, Never, Less, Equal, LEqual, Greater, NotEqual, GEqual, Always)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::BlendEquation, Add, Substract, ReverseSubstract, Min, Max)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::BlendFunction, Zero, One, SrcColor, InvSrcColor, DstColor, InvDstColor, SrcAlpha, InvSrcAlpha, DstAlpha, InvDstAlpha, Src1Color, InvSrc1Color, Src1Alpha, InvSrc1Alpha, SrcAlphaSat, CnstColor, InvCnstColor, CnstAlpha, InvCnstAlpha)

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Rasterization)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::DepthStencil)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Blend)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Stage)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::SubShader)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Settings)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Shader)
}

namespace VWolf {
    extern Ref<PShader> LoadPlatformShader(Shader& coreShader);
    extern std::vector<Ref<ShaderInput>> GetMaterialInputs(Ref<PShader> pshader);
    extern size_t GetMaterialSize(Ref<PShader> pshader);
    extern std::vector<ShaderInput> GetTextureInputs(Ref<PShader> pshader);

    std::vector<Ref<Shader>> Shader::m_shaders;
    std::map<Shader::ShaderSpecialty, std::string> Shader::m_specialtiesShaders;

    // TODO: Remove
    const char* Shader::CameraBufferName = "Camera";

    const char* Shader::ObjectBufferName = "Object";
    //

    Shader::Shader(std::filesystem::path path) {
        Deserialize(path);
    }

    void Shader::Deserialize(std::filesystem::path path) {
        constexpr const char * key = "shader";
        
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CORE_ERROR("Failed to load .material file '%s'\n     %s", path.string().c_str(), e.what());
        }
        (*this) = data[key].as<Shader>();
        internalShader = LoadPlatformShader(*this);
    }

    std::vector<Ref<ShaderInput>> Shader::GetMaterialInputs() const {
        return VWolf::GetMaterialInputs(internalShader);
    }

    size_t Shader::GetMaterialSize() const {
        return VWolf::GetMaterialSize(internalShader);
    }

    std::vector<ShaderInput> Shader::GetTextureInputs() const {
        return VWolf::GetTextureInputs(internalShader);
    }

    void Shader::LoadShader(std::filesystem::path path) {
        m_shaders.push_back(CreateRef<Shader>(path));
    }

    Ref<Shader> Shader::GetShader(std::string name) {
        for (auto shader: m_shaders) {
            std::string shaderName = shader->GetName();
            if (shaderName == name) {
                return shader;
            }
        }
        return nullptr;
    }

    Ref<Shader> Shader::GetShader(ShaderSpecialty type) {
        return GetShader(m_specialtiesShaders[type]);
    }

    void Shader::SetShaderSpecialty(std::string name, ShaderSpecialty type) {
        m_specialtiesShaders[type] = name;
    }
}
