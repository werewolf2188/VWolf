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
		HLSLShader(const std::string& name, BufferLayout layout, HWND__* window, DirectX12Context* context);
		virtual ~HLSLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const Vector2Float& value) override;
		virtual void SetFloat3(const std::string& name, const Vector3Float& value) override;
		virtual void SetFloat4(const std::string& name, const Vector4Float& value) override;
		virtual void SetMat3(const std::string& name, const MatrixFloat3x3& value) override;
		virtual void SetMat4(const std::string& name, const MatrixFloat4x4& value) override;

		virtual const std::string& GetName() const override;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipeline();
	private:
		HWND__* m_window;
		DirectX12Context* m_context;
		Ref<ShaderContext> m_shaderContext;
	};
}
#endif