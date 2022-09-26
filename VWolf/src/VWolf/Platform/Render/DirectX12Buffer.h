#pragma once

#include "VWolf/Core/Render/Buffer.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {

	struct VertexContext;

	class DirectX12VertexBuffer : public VertexBuffer
	{
	public:
		DirectX12VertexBuffer(HWND__* window, DirectX12Context* context, uint32_t size);
		DirectX12VertexBuffer(HWND__* window, DirectX12Context* context, float* vertices, uint32_t size);
		virtual ~DirectX12VertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override;
		virtual void SetLayout(const BufferLayout& layout) override;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	private:
		HWND__* m_window = nullptr;
		DirectX12Context* m_context = nullptr;
		Scope<VertexContext> m_vContext = nullptr;
		BufferLayout m_layout;
	};

	struct IndexContext;

	class DirectX12IndexBuffer : public IndexBuffer
	{
	public:
		DirectX12IndexBuffer(HWND__* window, DirectX12Context* context, uint32_t* indices, uint32_t count);
		virtual ~DirectX12IndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override;

		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;
	private:
		HWND__* m_window = nullptr;
		DirectX12Context* m_context = nullptr;
		Scope<IndexContext> m_iContext = nullptr;
	};
}