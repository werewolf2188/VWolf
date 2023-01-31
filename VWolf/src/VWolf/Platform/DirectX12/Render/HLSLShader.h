#pragma once

#include "VWolf/Core/Render/Shader.h"
#ifdef VWOLF_PLATFORM_WINDOWS

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;



namespace VWolf {
	struct ShaderContext;

	class HLSLShader : public Shader {
	public:
		HLSLShader(HWND__* window, 
				DirectX12Context* context,
				const char* name,
				ShaderSource vertexShader,
				BufferLayout layout,
				std::initializer_list<ShaderSource> otherShaders,
				std::vector<ShaderParameter> parameters,
				ShaderConfiguration configuration);
		virtual ~HLSLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const char* GetName() const override;
		virtual void SetData(const void* data, const char* name, uint32_t size, uint32_t offset = 0) override;
		virtual std::vector<Ref<ShaderInput>> GetMaterialInputs() const override;
		virtual size_t GetMaterialSize() const override;
		virtual std::vector<ShaderInput> GetTextureInputs() const override;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipeline();
	private:
		HWND__* m_window;
		DirectX12Context* m_context;
		Ref<ShaderContext> m_shaderContext;
	};
}
#endif
