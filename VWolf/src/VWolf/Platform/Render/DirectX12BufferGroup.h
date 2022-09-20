#pragma once

#include "VWolf/Core/Render/BufferGroup.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {
	class DirectX12BufferGroup : public BufferGroup
	{
	public:
		DirectX12BufferGroup(HWND__* window, DirectX12Context* context);
		virtual ~DirectX12BufferGroup();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override;
	private:
		HWND__* m_window;
		DirectX12Context* m_context;

		std::vector<Ref<VertexBuffer>> m_vBuffers;
		Ref<IndexBuffer> m_indexBuffer;
	};
}