//
//  Shader.cpp
//  VWolf
//
//  Created by Enrique Moises on 4/23/26.
//

#include "vwpch.h"
#include "Shader.h"

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::ShaderType, Vertex, Pre_Tesselator, Post_Tesselator, Geometry, Fragment)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::FillMode, Wireframe, Solid);
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::CullMode, Back, Front, FrontAndBack)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::DepthFunction, Never, Less, Equal, LEqual, Greater, NotEqual, GEqual, Always)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::BlendEquation, Add, Substract, ReverseSubstract, Min, Max)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::BlendFunction, Zero, One, SrcColor, InvSrcColor, DstColor, InvDstColor, SrcAlpha, InvSrcAlpha, DstAlpha, InvDstAlpha, Src1Color, InvSrc1Color, Src1Alpha, InvSrc1Alpha, SrcAlphaSat, CnstColor, InvCnstColor, CnstAlpha, InvCnstAlpha)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::PropertyType, Unknown, Integer, Float, Texture2D, Cubemap, Color, Vector)

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Rasterization)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::DepthStencil)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Blend)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Stage)
    template<>
    struct convert<VWolf::Property> {
        static bool decode(const Node& node, VWolf::Property& rhs)
        {
            DeserializeFromBoostDescribe(node, rhs);
            switch(rhs.type) {
                case VWolf::PropertyType::Integer:
                    rhs.value = node["value"] ? node["value"].as<uint32_t>() : 0;
                    break;
                case VWolf::PropertyType::Float:
                    rhs.value = node["value"] ? node["value"].as<float>() : 0;
                    break;
                case VWolf::PropertyType::Color:
                case VWolf::PropertyType::Vector:
                    rhs.value = node["value"] ? node["value"].as<VWolf::Vector4>() : VWolf::Vector4();
                    break;
                case VWolf::PropertyType::Texture2D:
                case VWolf::PropertyType::Cubemap:
                    rhs.value = node["value"] ? node["value"].as<std::string>() : "";
                    break;
                default:
                    VWOLF_CORE_ASSERT(false, "Shader Property failure: Wrong type");
                    break;
            }
            return true;
        }
    };
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::SubShader)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Settings)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Shader)
}

namespace VWolf {

    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(ShaderType, Vertex, Pre_Tesselator, Post_Tesselator, Geometry, Fragment)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(FillMode, Wireframe, Solid);
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(CullMode, Back, Front, FrontAndBack)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(DepthFunction, Never, Less, Equal, LEqual, Greater, NotEqual, GEqual, Always)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(BlendEquation, Add, Substract, ReverseSubstract, Min, Max)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(BlendFunction, Zero, One, SrcColor, InvSrcColor, DstColor, InvDstColor, SrcAlpha, InvSrcAlpha, DstAlpha, InvDstAlpha, Src1Color, InvSrc1Color, Src1Alpha, InvSrc1Alpha, SrcAlphaSat, CnstColor, InvCnstColor, CnstAlpha, InvCnstAlpha)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(PropertyType, Unknown, Integer, Float, Texture2D, Cubemap, Color, Vector)

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(Rasterization)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(DepthStencil)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(Blend)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(Stage)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(Property)
   
    YAML::Emitter& operator<<(YAML::Emitter& out, SubShader& v) {
        out << YAML::BeginMap;
        out << YAML::Key <<  "properties" << YAML::Value << v.properties;
        out << YAML::Key <<  "stages" << YAML::Value << v.stages;
        out << YAML::Key <<  "code" << YAML::Literal << v.code;
        out << YAML::EndMap;
        return out;
        
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const SubShader& v) {
        out << YAML::BeginMap;
        out << YAML::Key <<  "properties" << YAML::Value << v.properties;
        out << YAML::Key <<  "stages" << YAML::Value << v.stages;
        out << YAML::Key <<  "code" << YAML::Literal << v.code;
        out << YAML::EndMap;
        return out;
    }
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(Settings)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER(Shader)

    extern Ref<PShader> LoadPlatformShader(Shader& coreShader);
    extern std::vector<Ref<ShaderInput>> GetMaterialInputs(Ref<PShader> pshader);
    extern size_t GetMaterialSize(Ref<PShader> pshader);
    extern std::vector<ShaderInput> GetTextureInputs(Ref<PShader> pshader);

    std::vector<Ref<Shader>> Shader::m_shaders;
    std::vector<ShaderDependency> Shader::m_shader_dependencies;
    std::map<Shader::ShaderSpecialty, std::string> Shader::m_specialtiesShaders;

    // TODO: Remove
    const char* Shader::CameraBufferName = "Camera";

    const char* Shader::ObjectBufferName = "Object";
    //

    Shader::Shader(std::filesystem::path path, UUID _id): Object(_id) {
        Deserialize(path);
    }

    Shader::Shader(std::filesystem::path path, std::string newName): Object(UUID::NewUUID()) {
        Deserialize(path);
        name = newName;
    }

    Shader::Shader(const Shader& other): Object(other.id) {
        name = other.name;
        settings = other.settings;
        subShader = other.subShader;
        internalShader = other.internalShader;
    }

    Shader& Shader::operator=(const Shader& other) {
        id = other.id;
        name = other.name;
        settings = other.settings;
        subShader = other.subShader;
        internalShader = other.internalShader;
        
        return *this;
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

    void Shader::Save(std::filesystem::path path) const {
        YAML::Emitter out;
        out << *this;
        std::ofstream fout(path.string());
        fout << out.c_str();
    }

    void Shader::LoadShader(std::filesystem::path path, UUID _id) {
        Ref<Shader> shader = CreateRef<Shader>(path, _id);
        ObjectResourceManager::AddObject(_id, shader);
        m_shaders.push_back(shader);
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

    std::filesystem::path Shader::GetShaderLibraryPath(std::string filename) {
        auto pathIterator = std::find_if(
                                         m_shader_dependencies.begin(),
                                         m_shader_dependencies.end(),
                                         [filename](ShaderDependency& path) {
                                             std::filesystem::path filename_path = filename;
                                             return filename_path.filename() == path.GetPath().filename();
                                         });
        
        if (pathIterator == m_shader_dependencies.end()) return {};
        return (*pathIterator).GetPath();
    }

    void Shader::LoadShaderLibrary(std::filesystem::path path, UUID _id) {
        m_shader_dependencies.push_back(ShaderDependency(path, _id));
    }
}
