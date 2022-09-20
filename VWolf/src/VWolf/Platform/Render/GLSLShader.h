#pragma once

#include "VWolf/Core/Render/Shader.h"

struct GLFWwindow;

namespace VWolf {
	class GLSLShader : public Shader {
	public:
		GLSLShader(const std::string& name, BufferLayout layout, GLFWwindow* window);
		virtual ~GLSLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string & name, int value) override;
		virtual void SetIntArray(const std::string & name, int* values, uint32_t count) override;
		virtual void SetFloat(const std::string & name, float value) override;
		virtual void SetFloat2(const std::string & name, const Vector2Float & value) override;
		virtual void SetFloat3(const std::string & name, const Vector3Float & value) override;
		virtual void SetFloat4(const std::string & name, const Vector4Float & value) override;
		virtual void SetMat3(const std::string& name, const MatrixFloat3x3& value) override;
		virtual void SetMat4(const std::string & name, const MatrixFloat4x4 & value) override;

		virtual const std::string& GetName() const override;

		unsigned int GetProgramID() { return programId; }
	private:
		GLFWwindow* m_window;
		unsigned int programId;
	};
}