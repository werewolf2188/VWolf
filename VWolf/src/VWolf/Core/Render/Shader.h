//
//  Shader.hpp
//  VWolf
//
//  Created by Enrique Moises on 4/23/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Object.h"
#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Utils/GenericSerialization.h"

#include <any>

#include "yaml-cpp/yaml.h"
#include <boost/describe.hpp>

namespace VWolf {

    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
        case ShaderDataType::None:     return 0;
        }

        //VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!"); // ??????
        return 0;
    }

    // TODO: Clean
    // TODO: Move this eventually
    static std::string materialName = "Material";

    enum class ShaderSourceType {
        Text,
        File,
        Binary
    };

    // TODO: Should we use this for samplers as well
    enum class ShaderSamplerType: int {
        Sampler2D, SamplerCube
    };

    struct ShaderInput {
    public:
        ShaderInput(std::string name, ShaderDataType type, uint32_t index, uint32_t size, uint32_t offset):
        name(name), type(type), size(size), offset(offset), index(index) { }

        std::string GetName() { return name; }
        ShaderDataType GetType() { return type; }
        uint32_t GetIndex() { return index; }
        uint32_t GetSize() { return size; }
        uint32_t GetOffset() { return offset; }
    private:
        std::string name;
        ShaderDataType type;
        uint32_t size;
        uint32_t offset;
        uint32_t index;
    };
    //

    enum class ShaderType {
        Vertex = 1,
        Pre_Tesselator = 2,
        Post_Tesselator = 4,
        Geometry = 8,
        Fragment = 16,
        Compute = 32 // ????
    };

    BOOST_DESCRIBE_ENUM(ShaderType, Vertex, Pre_Tesselator, Post_Tesselator, Geometry, Fragment)

    enum class FillMode {
        Solid,
        Wireframe
    };

    BOOST_DESCRIBE_ENUM(FillMode, Solid, Wireframe)

    enum class CullMode {
        Back,
        Front,
        FrontAndBack
    };

    BOOST_DESCRIBE_ENUM(CullMode, Back, Front, FrontAndBack)

    enum class DepthFunction {
        Never,
        Less,
        Equal,
        LEqual,
        Greater,
        NotEqual,
        GEqual,
        Always
    };

    BOOST_DESCRIBE_ENUM(DepthFunction, Never, Less, Equal, LEqual, Greater, NotEqual, GEqual, Always)

    enum class BlendEquation {
        Add,
        Substract,
        ReverseSubstract,
        Min,
        Max
    };

    enum class BlendFunction {
        Zero, One,
        SrcColor, InvSrcColor,
        DstColor, InvDstColor,
        SrcAlpha, InvSrcAlpha,
        DstAlpha, InvDstAlpha,
        Src1Color, InvSrc1Color,
        Src1Alpha, InvSrc1Alpha,
        SrcAlphaSat,
        CnstColor, InvCnstColor,
        CnstAlpha, InvCnstAlpha
    };

    BOOST_DESCRIBE_ENUM(BlendFunction, Zero, One, SrcColor, InvSrcColor, DstColor, InvDstColor, SrcAlpha, InvSrcAlpha, DstAlpha, InvDstAlpha, Src1Color, InvSrc1Color, Src1Alpha, InvSrc1Alpha, SrcAlphaSat, CnstColor, InvCnstColor, CnstAlpha, InvCnstAlpha)

    BOOST_DESCRIBE_ENUM(BlendEquation, Add, Substract, ReverseSubstract, Min, Max)

    enum class PropertyType {
        Unknown,
        Integer,
        Float,
        Texture2D,
        Cubemap,
        Color,
        Vector
    };
    
    BOOST_DESCRIBE_ENUM(PropertyType, Unknown, Integer, Float, Texture2D, Cubemap, Color, Vector)

    class Property {
    public:
        Property() = default;
    public:
        const std::string& GetName() const { return name; }
        const std::string& GetDescription() const { return description; }
        const std::string& GetRelatedTo() const { return relatedTo; }
        const PropertyType& GetType() const { return type; }
        const bool& IsGamma() const { return gamma; }
        const bool& IsHDR() const { return hdr; }
        const bool& IsMainColor() const { return mainColor; }
        const bool& IsMainTexture() const { return mainTexture; }
        const bool& IsHidden() const { return hidden; }
        const bool& IsNormal() const { return normal; }
        bool IsTexture() {
            switch (type) {
                case PropertyType::Texture2D:
                case PropertyType::Cubemap:
                    return true;
                default: break;
            }
            return false;
        }
        bool IsVector() {
            switch (type) {
                case PropertyType::Color:
                case PropertyType::Vector:
                    return true;
                default: break;
            }
            return false;
        }
        bool IsScalar() {
            switch (type) {
                case PropertyType::Integer:
                case PropertyType::Float:
                    return true;
                default: break;
            }
            return false;
        }
        Vector2 GetRange() { return range; }
    public:
        template<typename T>
        T GetValue() {
            try {
                return std::any_cast<T>(value);
            } catch (const std::bad_any_cast& e) {
                throw e;
            }
        }
        
        template<typename T>
        bool TryGetValue(T& newValue) {
            try {
                newValue =  std::any_cast<T>(value);
                return true;
            } catch (const std::bad_any_cast& e) {
                return false;
            }
            return false;
        }
    private:
        std::string name;
        PropertyType type;
        std::any value;
        bool gamma = true;
        bool hdr = false;
        bool mainTexture = false;
        bool mainColor = false;
        bool noScaleOffset = false;
        bool normal = false;
        bool hidden = false;
        Vector2 range;
        std::string description;
        std::string relatedTo;
        BOOST_DESCRIBE_CLASS(Property, (), (), (), (name, type, gamma, hdr, mainTexture, mainColor, noScaleOffset, normal, hidden, range, description, relatedTo))
        VWOLF_SERIALIZATION_FRIENDS(Property)
        
        friend YAML::convert<Property>;
    };

    class Rasterization {
    public:
        Rasterization() = default;
    public:
        const bool GetCullEnabled() const { return cullEnabled; }
        const FillMode GetFillMode() const { return fillMode; }
        const CullMode GetCullMode() const { return cullMode; }
        const bool GetCounterClockwise() const { return counterClockwise; }
    private:
        bool cullEnabled = true;
        FillMode fillMode = FillMode::Solid;
        CullMode cullMode = CullMode::Back;
        bool counterClockwise = false;
        
        BOOST_DESCRIBE_CLASS(Rasterization, (), (), (), (cullEnabled, fillMode, cullMode, counterClockwise))
        VWOLF_SERIALIZATION_FRIENDS(Rasterization)
    };

    class DepthStencil {
    public:
        DepthStencil() = default;
    public:
        const bool GetDepthTest() const { return depthTest; }
        const DepthFunction GetDepthFunction() const { return depthFunction; }
    private:
        bool depthTest = true;
        DepthFunction depthFunction = DepthFunction::Less;
        
        BOOST_DESCRIBE_CLASS(DepthStencil, (), (), (), (depthTest, depthFunction))
        VWOLF_SERIALIZATION_FRIENDS(DepthStencil)
    };

    class Blend {
    public:
        Blend() = default;
    public:
        const bool GetEnabled() const { return enabled; }
        const BlendEquation GetEquation() const { return equation; }
        const BlendFunction GetSourceFunction() const { return sourceFunction; }
        const BlendFunction GetDestinationFunction() const { return destinationFunction; }
    private:
        bool enabled = true;
        BlendEquation equation = BlendEquation::Add;
        BlendFunction sourceFunction = BlendFunction::SrcAlpha;
        BlendFunction destinationFunction = BlendFunction::InvSrcAlpha;
        
        BOOST_DESCRIBE_CLASS(Blend, (), (), (), (equation, sourceFunction, destinationFunction))
        VWOLF_SERIALIZATION_FRIENDS(Blend)
    };

    class Settings {
    public:
        Settings() = default;
    public:
        const Rasterization GetRasterization() const { return rasterization; }
        const DepthStencil GetDepthStencil() const { return depthStencil; }
        const Blend GetBlend() const { return blend; }
    private:
        Rasterization rasterization = Rasterization();
        DepthStencil depthStencil = DepthStencil();
        Blend blend = Blend();
        BOOST_DESCRIBE_CLASS(Settings, (), (), (), (rasterization, depthStencil, blend))
        VWOLF_SERIALIZATION_FRIENDS(Settings)
    };

    class Stage {
    public:
        Stage() = default;
    public:
        ShaderType GetStageType() { return stageType; }
        const std::string& GetFunctionName() const { return functionName; }
    private:
        ShaderType stageType;
        std::string functionName;
        BOOST_DESCRIBE_CLASS(Stage, (), (), (), (stageType, functionName))
        VWOLF_SERIALIZATION_FRIENDS(Stage)
    };

    class SubShader {
    public:
        SubShader() = default;
    public:
        std::vector<Stage> GetStages() { return stages; }
        std::string GetCode() { return code; }
        std::vector<Property> GetProperties() { return properties; }
        const uint32_t GetPriority() const { return priority; }
    private:
        std::vector<Property> properties;
        std::vector<Stage> stages;
        std::string code;
        uint32_t priority;
        BOOST_DESCRIBE_CLASS(SubShader, (), (), (), (priority, properties, stages, code))
        VWOLF_SERIALIZATION_FRIENDS(SubShader)
    };

    struct ShaderDependency: public Object {
    public:
        ShaderDependency(std::filesystem::path m_path, UUID id): Object(id), m_path(m_path) {
        };
    public:
        std::filesystem::path& GetPath() { return m_path; }
    private:
        std::filesystem::path m_path;
    };

    class PShader;

    class Shader: public Object  {
    public:
        enum class ShaderSpecialty {
            shadow
        };
        
        // TODO: Not sure if this should live here.
        static const char* CameraBufferName;
        static const char* ObjectBufferName;
    public:
        Shader(): Object(UUID::NewUUID()) {};
        Shader(std::filesystem::path path, UUID _id);
        Shader(std::filesystem::path path, std::string newName);
        Shader(const Shader& other);
    public:
        SubShader GetSubShader() { return subShader; }
        Settings GetSettings() { return settings; }
        Ref<PShader> GetInternalShader() { return internalShader; }
    public:
        std::vector<Ref<ShaderInput>> GetMaterialInputs() const;
        size_t GetMaterialSize() const;
        std::vector<ShaderInput> GetTextureInputs() const;
        void Save(std::filesystem::path path) const;
    public:
        Shader& operator=(const Shader& other);
    public:
        static void LoadShader(std::filesystem::path path, UUID _id);
        static void LoadShaderLibrary(std::filesystem::path path, UUID _id);
        static Ref<Shader> GetShader(std::string name);
        static Ref<Shader> GetShader(ShaderSpecialty type);
        static std::filesystem::path GetShaderLibraryPath(std::string filename);
        static void SetShaderSpecialty(std::string name, ShaderSpecialty type);
    private:
        void Deserialize(std::filesystem::path path);
    private:
        Settings settings;
        SubShader subShader;
        
        BOOST_DESCRIBE_CLASS(Shader, (), (), (name), (settings, subShader))
        VWOLF_SERIALIZATION_FRIENDS(Shader)
        
        static std::vector<Ref<Shader>> m_shaders;
        static std::vector<ShaderDependency> m_shader_dependencies;
        static std::map<ShaderSpecialty, std::string> m_specialtiesShaders;
        
        Ref<PShader> internalShader;
        friend class PShader;
    };
}
