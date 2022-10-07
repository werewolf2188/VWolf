#include "vwpch.h"
#include "GLSLShader.h"

#include "VWolf/Core/Utils/File.h"

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MESSAGE_LENGTH 512

namespace VWolf {

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
                           std::initializer_list<ShaderParameter> parameters,
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
        GLuint uniformBufferId;
        glGenBuffers(m_parameters.size(), &uniformBufferId);
        GLuint* ptr = &uniformBufferId;
        int index = 0;

        for (ShaderParameter param: m_parameters) {
            unsigned int uniformBlock = glGetUniformBlockIndex(programId, param.name);
            glUniformBlockBinding(programId, uniformBlock, param.binding);
    
            glBindBuffer(GL_UNIFORM_BUFFER, *(ptr + index));
            glBufferData(GL_UNIFORM_BUFFER, param.size, nullptr, GL_STATIC_DRAW); // TODO: investigate usage hint
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, param.binding, *(ptr + index));
            m_uniformBuffers.insert(std::pair<const char*, int>(param.name, *(ptr + index)));
            index++;
        }
        
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
