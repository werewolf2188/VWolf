#pragma once

#include "VWolf/Core/Render/RenderAPI.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {
	class DirectX12RenderAPI : public RenderAPI {
	public:
		DirectX12RenderAPI(HWND__* window, DirectX12Context* context);

		virtual void Begin(Camera& camera, Ref<Shader> shader) override;
		virtual void ClearColor(Color color) override;
		virtual void Clear() override;
		virtual void End() override;
		virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
		virtual void DrawIndexed(const Ref<BufferGroup>& group, uint32_t indexCount) override;
	public:
		HWND__* m_window;
		DirectX12Context* context;
	};
}