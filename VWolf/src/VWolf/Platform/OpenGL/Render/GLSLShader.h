#pragma once

#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Platform/PShader.h"

struct GLFWwindow;

namespace VWolf {
    class GLProgram;

	class [[deprecated("This class is deprecated. Use HLSLOpenGLShader instead")]] GLSLShader : public PShader {
	public:
        // TODO: Compute shader is the only one that is different.
        GLSLShader(std::string name,
                   std::initializer_list<ShaderSource> otherShaders,
                   ShaderConfiguration configuration = {});
        GLSLShader(Shader& coreShader);
		virtual ~GLSLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
        virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const override;
        virtual size_t GetMaterialSize() const override;
        virtual std::vector<ShaderInput> GetTextureInputs() const override;

		virtual std::string GetName() const override;
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
