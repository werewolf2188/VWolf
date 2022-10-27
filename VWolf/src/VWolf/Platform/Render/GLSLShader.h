#pragma once

#include "VWolf/Core/Render/Shader.h"

struct GLFWwindow;

namespace VWolf {
	class GLSLShader : public Shader {
	public:
		GLSLShader(GLFWwindow* window,
                   const char* name,
                   ShaderSource vertexShader,
                   BufferLayout layout,
                   std::initializer_list<ShaderSource> otherShaders,
                   std::vector<ShaderParameter> parameters,
                   ShaderConfiguration configuration);
		virtual ~GLSLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const char* GetName() const override;
        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) override;

		unsigned int GetProgramID() { return programId; }
    private:
        void SetConfiguration() const;
        void SetRasterization() const;
        void SetBlend() const;
        void SetDepthStencil() const;
	private:
		GLFWwindow* m_window;
		unsigned int programId;
        std::map<const char*, int> m_uniformBuffers;

    private:
        static int totalParams;
	};
}
