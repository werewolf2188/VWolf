#include "vwpch.h"
#include "GLSLShader.h"

#include "VWolf/Core/Utils/File.h"

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MESSAGE_LENGTH 512

namespace VWolf {

    int GLSLShader::totalParams = 0;

    GLuint ShaderTypeEquivalent(ShaderType type) {
        switch (type) {
            case ShaderType::Vertex: return GL_VERTEX_SHADER;
            case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
            case ShaderType::Pre_Tesselator: return GL_TESS_CONTROL_SHADER;
            case ShaderType::Post_Tesselator: return GL_TESS_EVALUATION_SHADER;
            case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
            case ShaderType::Compute: return GL_COMPUTE_SHADER;
        }
    }

    GLuint CompileShader(const char* shaderText, int shaderType) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderText, NULL);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar infoLog[MESSAGE_LENGTH];
            glGetShaderInfoLog(shader, MESSAGE_LENGTH, nullptr, infoLog);
            VWOLF_CORE_DEBUG(infoLog);
            VWOLF_CORE_ASSERT(success, "Something happened");
            return 0;
        }
        return shader;
    }

    GLuint CompileShader(ShaderSource source) {
        std::string shaderCode;
        switch (source.sourceType) {
            case ShaderSourceType::File:
                shaderCode = File::OpenTextFile(source.shader);
                break;
            case ShaderSourceType::Text:
                shaderCode = source.shader;
                break;
            case ShaderSourceType::Binary:
                VWOLF_CORE_ASSERT(false, "Shaders cannot be binary at the moment");
                break;
        }
        GLuint shaderId = CompileShader(shaderCode.c_str(), ShaderTypeEquivalent(source.type));
        VWOLF_CORE_ASSERT(shaderId, "Shader didn't compile");
        return shaderId;
    }

	GLSLShader::GLSLShader(GLFWwindow* window,
                           const char* name,
                           ShaderSource vertexShader,
                           BufferLayout layout,
                           std::initializer_list<ShaderSource> otherShaders,
                           std::vector<ShaderParameter> parameters,
                           ShaderConfiguration configuration): m_window(window), Shader(name,
                                                                                        vertexShader,
                                                                                        layout,
                                                                                        otherShaders,
                                                                                        parameters,
                                                                                        configuration)

	{
        VWOLF_CORE_ASSERT(vertexShader.type == ShaderType::Vertex, "The first shader should be a vertex shader");
        bool hasFragmentShader = false;
        for(ShaderSource source: m_otherShaders) {
            hasFragmentShader = source.type == ShaderType::Fragment;
            if (hasFragmentShader) break;
        }
        VWOLF_CORE_ASSERT(hasFragmentShader, "There should be at least one fragment shader");
        std::vector<GLuint> shaders(m_otherShaders.size() + 1);
        shaders.push_back(CompileShader(vertexShader));
        for(ShaderSource source: m_otherShaders)
            shaders.push_back(CompileShader(source));

		programId = glCreateProgram();
        for(GLuint shader: shaders)
            glAttachShader(programId, shader);
		glLinkProgram(programId);

		GLint success;
		glGetProgramiv(programId, GL_LINK_STATUS, &success);
		if (!success) {
			GLchar infoLog[MESSAGE_LENGTH];
			glGetProgramInfoLog(programId, MESSAGE_LENGTH, nullptr, infoLog);
			VWOLF_CORE_DEBUG(infoLog);
			VWOLF_CORE_ASSERT(false, infoLog);
		}

        // Uniform blocks
        GLuint* uniformBuffers = new GLuint[m_parameters.size()]; //(GLuint*)malloc(m_parameters.size() * sizeof(GLuint));
        glGenBuffers(m_parameters.size(), uniformBuffers);
        int index = 0;

        for (ShaderParameter param: m_parameters) {
            unsigned int uniformBlock = glGetUniformBlockIndex(programId, param.name);
            glUniformBlockBinding(programId, uniformBlock, param.binding + totalParams);
    
            glBindBuffer(GL_UNIFORM_BUFFER, *(uniformBuffers + index));
            glBufferData(GL_UNIFORM_BUFFER, param.size, nullptr, GL_STATIC_DRAW); // TODO: investigate usage hint
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, param.binding + totalParams, *(uniformBuffers + index));
            m_uniformBuffers.insert(std::pair<const char*, int>(param.name, *(uniformBuffers + index)));
            index++;
        }
        totalParams += index;
	}
	GLSLShader::~GLSLShader()
	{
        for (ShaderParameter param: m_parameters) {
            GLuint uniformBufferId = m_uniformBuffers[param.name];
            glDeleteBuffers(1, &uniformBufferId);
        }
		glDeleteProgram(programId);
	}
	void GLSLShader::Bind() const
	{
		glUseProgram(programId);
        // Configuration
        SetConfiguration();

        switch (m_configuration.blend.equation) {
            case ShaderConfiguration::Blend::Equation::Add:
                glBlendEquation(GL_FUNC_ADD);
                break;
            case ShaderConfiguration::Blend::Equation::Substract:
                glBlendEquation(GL_FUNC_SUBTRACT);
                break;
            case ShaderConfiguration::Blend::Equation::ReverseSubstract:
                glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
                break;
            case ShaderConfiguration::Blend::Equation::Min:
                glBlendEquation(GL_MIN);
                break;
            case ShaderConfiguration::Blend::Equation::Max:
                glBlendEquation(GL_MAX);
                break;
        }
	}

    void GLSLShader::SetConfiguration() const {
        SetRasterization();
        SetBlend();
        SetDepthStencil();
    }

    void GLSLShader::SetRasterization() const {
        // Rasterization
        if (m_configuration.rasterization.cullEnabled)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        switch (m_configuration.rasterization.cullMode) {
            case ShaderConfiguration::Rasterization::CullMode::Front:
                glCullFace(GL_FRONT);
                break;
            case ShaderConfiguration::Rasterization::CullMode::Back:
                glCullFace(GL_BACK);
                break;
            case ShaderConfiguration::Rasterization::CullMode::FrontAndBack:
                glCullFace(GL_FRONT_AND_BACK);
                break;
        }

        switch (m_configuration.rasterization.fillMode) {
            case ShaderConfiguration::Rasterization::FillMode::Wireframe:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case ShaderConfiguration::Rasterization::FillMode::Solid:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
        }

        glFrontFace(m_configuration.rasterization.counterClockwise ? GL_CCW: GL_CW);

    }

    GLuint GetBlendFunction(ShaderConfiguration::Blend::Function function) {
        switch (function) {
            case ShaderConfiguration::Blend::Function::Zero: return GL_ZERO;
            case ShaderConfiguration::Blend::Function::One: return GL_ONE;
            case ShaderConfiguration::Blend::Function::SrcColor: return GL_SRC_COLOR;
            case ShaderConfiguration::Blend::Function::InvSrcColor: return GL_ONE_MINUS_SRC_COLOR;
            case ShaderConfiguration::Blend::Function::DstColor: return GL_DST_COLOR;
            case ShaderConfiguration::Blend::Function::InvDstColor: return GL_ONE_MINUS_DST_COLOR;
            case ShaderConfiguration::Blend::Function::SrcAlpha: return GL_SRC_ALPHA;
            case ShaderConfiguration::Blend::Function::InvSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case ShaderConfiguration::Blend::Function::DstAlpha: return GL_DST_ALPHA;
            case ShaderConfiguration::Blend::Function::InvDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
            case ShaderConfiguration::Blend::Function::Src1Color: return GL_SRC1_COLOR;
            case ShaderConfiguration::Blend::Function::InvSrc1Color: return GL_ONE_MINUS_SRC1_COLOR;
            case ShaderConfiguration::Blend::Function::Src1Alpha: return GL_SRC1_ALPHA;
            case ShaderConfiguration::Blend::Function::InvSrc1Alpha: return GL_ONE_MINUS_SRC1_ALPHA;
            case ShaderConfiguration::Blend::Function::SrcAlphaSat: return GL_SRC_ALPHA_SATURATE;
            case ShaderConfiguration::Blend::Function::CnstColor: return GL_CONSTANT_COLOR;
            case ShaderConfiguration::Blend::Function::InvCnstColor: return GL_ONE_MINUS_CONSTANT_COLOR;
            case ShaderConfiguration::Blend::Function::CnstAlpha: return GL_CONSTANT_ALPHA;
            case ShaderConfiguration::Blend::Function::InvCnstAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
        }
        return GL_ZERO;
    }

    void GLSLShader::SetBlend() const {
        // Blend
        if (m_configuration.blend.enabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        GLuint sourceFunction, destinationFunction;
        sourceFunction = GetBlendFunction(m_configuration.blend.sourceFunction);
        destinationFunction = GetBlendFunction(m_configuration.blend.destinationFunction);
        glBlendFunc(sourceFunction, destinationFunction);
        
        switch (m_configuration.blend.equation) {
            case ShaderConfiguration::Blend::Equation::Add:
                glBlendEquation(GL_FUNC_ADD);
                break;
            case ShaderConfiguration::Blend::Equation::Substract:
                glBlendEquation(GL_FUNC_SUBTRACT);
                break;
            case ShaderConfiguration::Blend::Equation::ReverseSubstract:
                glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
                break;
            case ShaderConfiguration::Blend::Equation::Min:
                glBlendEquation(GL_MIN);
                break;
            case ShaderConfiguration::Blend::Equation::Max:
                glBlendEquation(GL_MAX);
                break;
        }
    }

    void GLSLShader::SetDepthStencil() const {
        // Depth/Stencil
        if (m_configuration.depthStencil.depthTest)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

	void GLSLShader::Unbind() const
	{
		glUseProgram(0);
	}

	const char* GLSLShader::GetName() const
	{
		return this->m_name;
	}

    void GLSLShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
        GLuint uniformBufferId = m_uniformBuffers[name];
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}
