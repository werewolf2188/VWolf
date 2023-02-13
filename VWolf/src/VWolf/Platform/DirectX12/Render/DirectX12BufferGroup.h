#pragma once

#include "VWolf/Core/Render/RenderStructs.h"
#include "DirectX12Buffer.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {
	class DirectX12BufferGroup
	{
	public:
		DirectX12BufferGroup(DirectX12Context* context);
		~DirectX12BufferGroup();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const Ref<DirectX12VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<DirectX12IndexBuffer>& indexBuffer);

		const std::vector<Ref<DirectX12VertexBuffer>>& GetVertexBuffers() const;
		const Ref<DirectX12IndexBuffer>& GetIndexBuffer() const;
	private:
		DirectX12Context* m_context;

		std::vector<Ref<DirectX12VertexBuffer>> m_vBuffers;
		Ref<DirectX12IndexBuffer> m_indexBuffer;
	};
}