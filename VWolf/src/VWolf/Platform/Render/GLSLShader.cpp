#include "vwpch.h"
#include "GLSLShader.h"

#include "VWolf/Core/Utils/File.h"

#include "VWolf/Platform/Utils/GLMConversions.h"

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {

	GLuint CompileShader(const char* shaderText, int shaderType) {
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderText, NULL);
		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar infoLog[512];
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			VWOLF_CORE_DEBUG(infoLog);
			return 0;
		}
		return shader;
	}

	GLSLShader::GLSLShader(const std::string& name, BufferLayout layout, GLFWwindow* window): Shader(name, layout), m_window(window)
	{		
		auto vertexShaderFile = File::OpenTextFile((filepath + ".vert.glsl").c_str());
		auto fragmentShaderFile = File::OpenTextFile((filepath + ".frag.glsl").c_str());

		auto vertexShader_text = vertexShaderFile.c_str();
		auto fragmentShader_text = fragmentShaderFile.c_str();
		/*VWOLF_CORE_DEBUG(vertexShader_text);
		VWOLF_CORE_DEBUG(fragmentShader_text);*/
		
		GLuint vertex_shader = CompileShader(vertexShader_text, GL_VERTEX_SHADER);
		VWOLF_CORE_ASSERT(vertex_shader, "Vertex Shader didn't compile");
		GLuint fragment_shader = CompileShader(fragmentShader_text, GL_FRAGMENT_SHADER);
		VWOLF_CORE_ASSERT(fragment_shader, "Fragment Shader didn't compile");

		programId = glCreateProgram();
		glAttachShader(programId, vertex_shader);
		glAttachShader(programId, fragment_shader);
		glLinkProgram(programId);

		GLint success;
		glGetProgramiv(programId, GL_LINK_STATUS, &success);
		if (!success) {
			GLchar infoLog[512];
			glGetProgramInfoLog(programId, 512, nullptr, infoLog);
			VWOLF_CORE_DEBUG(infoLog);
			VWOLF_CORE_ASSERT(false, infoLog);
		}
	}
	GLSLShader::~GLSLShader()
	{
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
	void GLSLShader::SetInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniform1i(location, value);
	}
	void GLSLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniform1iv(location, count, values);
	}
	void GLSLShader::SetFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniform1f(location, value);
	}
	void GLSLShader::SetFloat2(const std::string& name, const Vector2Float& value)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniform2f(location, value.x, value.y);
	}
	void GLSLShader::SetFloat3(const std::string& name, const Vector3Float& value)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}
	void GLSLShader::SetFloat4(const std::string& name, const Vector4Float& value)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void GLSLShader::SetMat3(const std::string& name, const MatrixFloat3x3& value)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(toGLMMatrix3x3(value)));
	}
	void GLSLShader::SetMat4(const std::string& name, const MatrixFloat4x4& value)
	{
		GLint location = glGetUniformLocation(programId, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(toGLMMatrix4x4(value)));
	}
	const std::string& GLSLShader::GetName() const
	{
		return this->filepath;
	}
}