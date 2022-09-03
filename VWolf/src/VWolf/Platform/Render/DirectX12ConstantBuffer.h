#pragma once

#include "VWolf/Core/Render/UniformBuffer.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {

	struct ConstantBufferContext;

	class DirectX12ConstantBuffer : public UniformBuffer {
	public:
		DirectX12ConstantBuffer(HWND__* window, DirectX12Context* context, Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding);
		virtual ~DirectX12ConstantBuffer() override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:
		HWND__* m_window = nullptr;
		DirectX12Context* m_context = nullptr;
		ConstantBufferContext* m_cbContext = nullptr;
	};
}