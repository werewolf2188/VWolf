#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"

#include "RenderStructs.h"

#include <functional>

namespace VWolf {

    // TODO: Move this eventually
    static std::string materialName = "Material";

    enum class ShaderType {
        Vertex = 1,
        Pre_Tesselator = 2,
        Post_Tesselator = 4,
        Geometry = 8,
        Fragment = 16,
        Compute = 32 // ????
    };

    enum class ShaderSourceType {
        Text,
        File,
        Binary
    };

    // TODO: Should we use this for samplers as well
    enum class ShaderSamplerType: int {
        Sampler2D, SamplerCube
    };

    struct ShaderSource {
        ShaderType type;
        ShaderSourceType sourceType;
        std::string shader;
        const char* mainFunction = "main";
    };

    struct ShaderConfiguration {
        struct Rasterization {
            bool cullEnabled = true;
            enum class FillMode { Wireframe, Solid };
            enum class CullMode { Front, Back, FrontAndBack };
            
            FillMode fillMode = FillMode::Solid;
            CullMode cullMode = CullMode::Back;
            bool counterClockwise = false;
        };

        struct DepthStencil {
            bool depthTest = true;
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

            DepthFunction depthFunction = DepthFunction::Less;
        };

        struct Blend {
            enum class Equation { Add, Substract, ReverseSubstract, Min, Max };
            enum class Function {
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
            
            bool enabled = true;
            Equation equation = Equation::Add;
            Function sourceFunction = Function::SrcAlpha;
            Function destinationFunction = Function::InvSrcAlpha;
        };
        Rasterization rasterization = Rasterization();
        DepthStencil depthStencil = DepthStencil();
        Blend blend = Blend();
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

	class Shader {
	public:
        Shader(std::string name,
               std::initializer_list<ShaderSource> otherShaders,
               ShaderConfiguration configuration):
        m_name(name),
        m_otherShaders(otherShaders),
        m_configuration(configuration) {}

		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const = 0;
        virtual size_t GetMaterialSize() const = 0;
        virtual std::vector<ShaderInput> GetTextureInputs() const = 0;

		virtual std::string GetName() const = 0;

        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) = 0;
	protected:
        std::string m_name;
        std::vector<ShaderSource> m_otherShaders;
        ShaderConfiguration m_configuration;
	};

    class ShaderLibrary {
    public:
        // TODO: Not sure if this should live here.
        static const char* CameraBufferName;
        static const char* ObjectBufferName;

        static void LoadShader(std::string name,
                               std::initializer_list<ShaderSource> otherShaders,
                               ShaderConfiguration configuration = {});

        static Ref<Shader> GetShader(const char* name);
    private:
        static std::vector<Ref<Shader>> m_shaders;
    };
}
