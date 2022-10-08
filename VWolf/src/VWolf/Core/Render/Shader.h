#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/Math.h"

#include "Buffer.h"

#include <functional>

namespace VWolf {

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
        
    };

	class Shader {
	public:
        Shader(const char* name,
               ShaderSource vertexShader,
               BufferLayout layout,
               std::initializer_list<ShaderSource> otherShaders,
               std::initializer_list<ShaderParameter> parameters,
               ShaderConfiguration configuration):
        m_name(name),
        m_vertexShader(vertexShader),
        m_layout(layout),
        m_otherShaders(otherShaders),
        m_parameters(parameters),
        m_configuration(configuration) {}

		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const char* GetName() const = 0;

        // TODO: Remove
		void SetLayout(const BufferLayout& layout) { m_layout = layout; };
        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<Shader> Create(const char* name,
                                  ShaderSource vertexShader,
                                  BufferLayout layout,
                                  std::initializer_list<ShaderSource> otherShaders,
                                  std::initializer_list<ShaderParameter> parameters,
                                  ShaderConfiguration configuration) {
			return m_create(name, vertexShader, layout, otherShaders, parameters, configuration);
		}
#ifdef VWOLF_CORE
		static void SetDefaultCreateMethod(std::function<Ref<Shader>(const char* name,
                                                                     ShaderSource vertexShader,
                                                                     BufferLayout layout,
                                                                     std::initializer_list<ShaderSource> otherShaders,
                                                                     std::initializer_list<ShaderParameter> parameters,
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
                                         std::initializer_list<ShaderParameter> parameters,
                                         ShaderConfiguration configuration)> m_create;
	};

	inline std::function<Ref<Shader>(const char* name,
                                     ShaderSource vertexShader,
                                     BufferLayout layout,
                                     std::initializer_list<ShaderSource> otherShaders,
                                     std::initializer_list<ShaderParameter> parameters,
                                     ShaderConfiguration configuration)> Shader::m_create = nullptr;
}
