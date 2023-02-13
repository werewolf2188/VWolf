#pragma once

#include "VWolf/Core/Render/RenderStructs.h"

#ifdef VWOLF_PLATFORM_WINDOWS

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {

	struct VertexContext;

	class DirectX12VertexBuffer
	{
	public:
		DirectX12VertexBuffer(DirectX12Context* context, uint32_t size);
		DirectX12VertexBuffer(DirectX12Context* context, void* vertices, uint32_t size);
		~DirectX12VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t size);

		const BufferLayout& GetLayout() const;
		void SetLayout(const BufferLayout& layout);

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
	private:
		DirectX12Context* m_context = nullptr;
		Scope<VertexContext> m_vContext = nullptr;
		BufferLayout m_layout;
		void* m_vertices;
		uint32_t m_size;
	};

	struct IndexContext;

	class DirectX12IndexBuffer
	{
	public:
		DirectX12IndexBuffer(DirectX12Context* context, uint32_t* indices, uint32_t count);
		virtual ~DirectX12IndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetCount() const;

		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;
	private:
		DirectX12Context* m_context = nullptr;
		Scope<IndexContext> m_iContext = nullptr;
		uint32_t* m_indices;
		uint32_t m_count;
	};
}
#endif