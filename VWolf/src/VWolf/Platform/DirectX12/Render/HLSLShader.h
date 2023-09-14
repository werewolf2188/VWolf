#pragma once

#include "VWolf/Core/Render/Shader.h"
#ifdef VWOLF_PLATFORM_WINDOWS
namespace VWolf {
	class HLProgram;

	class HLSLShader : public Shader {
	public:
		HLSLShader(std::string name,
				   std::initializer_list<ShaderSource> otherShaders,
				   ShaderConfiguration configuration = {});
		virtual ~HLSLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual std::string GetName() const override;
		virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) override;
		virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const override;
		virtual size_t GetMaterialSize() const override;
		virtual std::vector<ShaderInput> GetTextureInputs() const override;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipeline();
	private:
		Ref<HLProgram> m_program;
	};
}
#endif
