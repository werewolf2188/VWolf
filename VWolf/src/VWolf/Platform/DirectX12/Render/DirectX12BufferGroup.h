#pragma once

#include "VWolf/Core/Render/RenderStructs.h"
#include "DirectX12Buffer.h"

namespace VWolf {
	class DX12Command;

	class DirectX12BufferGroup
	{
	public:
		DirectX12BufferGroup();
		~DirectX12BufferGroup();

		void Bind(Ref<DX12Command> commands) const;

		void SetVertexBuffer(const Ref<DirectX12VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<DirectX12IndexBuffer>& indexBuffer);

		const Ref<DirectX12VertexBuffer>& GetVertexBuffers() const { return m_vBuffer; }
		const Ref<DirectX12IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }
	private:
		Ref<DirectX12VertexBuffer> m_vBuffer;
		Ref<DirectX12IndexBuffer> m_indexBuffer;
	};
}