#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/Math.h"

#include "RenderStructs.h"

#include <functional>

namespace VWolf {

    // TODO: Move this eventually
    static std::string materialName = "Material";

    enum class ShaderType {
        Vertex,
        Pre_Tesselator,
        Post_Tesselator,
        Geometry,
        Fragment,
        Compute
    };

    enum class ShaderSourceType {
        Text,
        File,
        Binary
    };

    enum class ShaderParameterType {
        In,
        Out
    };

    struct ShaderSource {
        ShaderType type;
        ShaderSourceType sourceType;
        const char* shader;
        const char* mainFunction = "main";
    };

    struct ShaderParameter {
        const char* name;
        ShaderParameterType type;
        int binding;
        size_t size;
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
        Shader(const char* name,
               ShaderSource vertexShader,
               BufferLayout layout,
               std::initializer_list<ShaderSource> otherShaders,
               std::vector<ShaderParameter> parameters,
               ShaderConfiguration configuration):
        m_name(name),
        m_vertexShader(vertexShader),
        m_layout(layout),
        m_otherShaders(otherShaders),
        m_parameters(parameters),
        m_configuration(configuration) {}

        Shader(const char* name,
               BufferLayout layout,
               std::initializer_list<ShaderSource> otherShaders,
               ShaderConfiguration configuration):
        m_name(name),
        m_layout(layout),
        m_otherShaders(otherShaders),
        m_configuration(configuration) {}

		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const = 0;
        virtual size_t GetMaterialSize() const = 0;
        virtual std::vector<ShaderInput> GetTextureInputs() const = 0;

		virtual const char* GetName() const = 0;

        // TODO: Remove
		void SetLayout(const BufferLayout& layout) { m_layout = layout; };
        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<Shader> Create(const char* name,
                                  ShaderSource vertexShader,
                                  BufferLayout layout,
                                  std::initializer_list<ShaderSource> otherShaders,
                                  std::vector<ShaderParameter> parameters,
                                  ShaderConfiguration configuration) {
			return m_create(name, vertexShader, layout, otherShaders, parameters, configuration);
		}
#ifdef VWOLF_CORE
		static void SetDefaultCreateMethod(std::function<Ref<Shader>(const char* name,
                                                                     ShaderSource vertexShader,
                                                                     BufferLayout layout,
                                                                     std::initializer_list<ShaderSource> otherShaders,
                                                                     std::vector<ShaderParameter> parameters,
                                                                     ShaderConfiguration configuration)> create) { m_create = create; }
#endif
	protected:
        const char* m_name;
        ShaderSource m_vertexShader;
		BufferLayout m_layout;
        std::vector<ShaderSource> m_otherShaders;
        std::vector<ShaderParameter> m_parameters;
        ShaderConfiguration m_configuration;
	private:
		static std::function<Ref<Shader>(const char* name,
                                         ShaderSource vertexShader,
                                         BufferLayout layout,
                                         std::initializer_list<ShaderSource> otherShaders,
                                         std::vector<ShaderParameter> parameters,
                                         ShaderConfiguration configuration)> m_create;
	};

	inline std::function<Ref<Shader>(const char* name,
                                     ShaderSource vertexShader,
                                     BufferLayout layout,
                                     std::initializer_list<ShaderSource> otherShaders,
                                     std::vector<ShaderParameter> parameters,
                                     ShaderConfiguration configuration)> Shader::m_create = nullptr;
    class ShaderLibrary {
    public:
        // TODO: Not sure if this should live here.
        static const char* CameraBufferName;
        static const char* ObjectBufferName;

        static void LoadShader(const char* name,
                               ShaderSource vertexShader,
                               std::initializer_list<ShaderSource> otherShaders,
                               std::initializer_list<ShaderParameter> parameters,
            ShaderConfiguration configuration = {}) {
            
            std::vector<VWolf::ShaderParameter> minimalParameters = {
             { ShaderLibrary::CameraBufferName, VWolf::ShaderParameterType::In, 0, sizeof(VWolf::CameraPass) },
             { ShaderLibrary::ObjectBufferName, VWolf::ShaderParameterType::In, 1, sizeof(VWolf::MatrixFloat4x4) }
            };
            
            minimalParameters.insert(minimalParameters.end(), parameters.begin(), parameters.end());
            m_shaders.push_back(Shader::Create(name, vertexShader, MeshData::Layout, otherShaders, minimalParameters, configuration));
        }

        static void LoadShader(const char* name,
                               std::initializer_list<ShaderSource> otherShaders,
                               ShaderConfiguration configuration = {});

        static Ref<Shader> GetShader(const char* name);
    private:
        static std::vector<Ref<Shader>> m_shaders;
    };
}
