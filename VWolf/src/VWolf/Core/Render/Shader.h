//
//  Shader.hpp
//  VWolf
//
//  Created by Enrique Moises on 4/23/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"

#include "RenderStructs.h"

#include <functional>

#include "yaml-cpp/yaml.h"
#include <boost/describe.hpp>

namespace VWolf {

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
    };

    class SubShader {
    public:
        SubShader() = default;
    public:
        std::vector<Stage> GetStages() { return stages; }
        std::string GetCode() { return code; }
    private:
        std::vector<Stage> stages;
        std::string code;
        BOOST_DESCRIBE_CLASS(SubShader, (), (), (), (code, stages))
    };

    class PShader;

    class Shader {
    public:
        enum class ShaderSpecialty {
            shadow
        };
        
        // TODO: Not sure if this should live here.
        static const char* CameraBufferName;
        static const char* ObjectBufferName;
    public:
        Shader() = default;
        Shader(std::filesystem::path path);
    public:
        const std::string GetName() const { return name; }
        SubShader GetSubShader() { return subShader; }
        Settings GetSettings() { return settings; }
        Ref<PShader> GetInternalShader() { return internalShader; }
    public:
        std::vector<Ref<ShaderInput>> GetMaterialInputs() const;
        size_t GetMaterialSize() const;
        std::vector<ShaderInput> GetTextureInputs() const;
    public:
        static void LoadShader(std::filesystem::path path);
        static Ref<Shader> GetShader(std::string name);
        static Ref<Shader> GetShader(ShaderSpecialty type);
        static void SetShaderSpecialty(std::string name, ShaderSpecialty type);
    private:
        void Deserialize(std::filesystem::path path);
    private:
        std::string name;
        Settings settings;
        SubShader subShader;
        
        BOOST_DESCRIBE_CLASS(Shader, (), (), (), (name, settings, subShader))
        
        static std::vector<Ref<Shader>> m_shaders;
        static std::map<ShaderSpecialty, std::string> m_specialtiesShaders;
        
        Ref<PShader> internalShader;
        friend class PShader;
    };
}
