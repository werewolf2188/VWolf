#pragma once

#include "VWolf/Core/Render/Shader.h"

struct GLFWwindow;

namespace VWolf {
    class GLProgram;

	class GLSLShader : public Shader {
	public:
        // TODO: We don't need to separate both shaders
        // TODO: Compute shader is the only one that is different.
        // TODO: Maybe, buffer layout can be created by the shader
		GLSLShader(const char* name,
                   ShaderSource vertexShader,
                   BufferLayout layout,
                   std::initializer_list<ShaderSource> otherShaders,
                   std::vector<ShaderParameter> parameters,
                   ShaderConfiguration configuration);
        GLSLShader(const char* name,
                   BufferLayout layout,
                   std::initializer_list<ShaderSource> otherShaders,
                   ShaderConfiguration configuration);
		virtual ~GLSLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const override;
        virtual size_t GetMaterialSize() const override;

		virtual const char* GetName() const override;
        virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) override;
    private:
        void SetConfiguration() const;
        void SetRasterization() const;
        void SetBlend() const;
        void SetDepthStencil() const;
	private:
        Ref<GLProgram> m_program;
	};
}
