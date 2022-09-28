#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12BufferGroup.h"

#include "DirectX12Buffer.h"

namespace VWolf {
	DirectX12BufferGroup::DirectX12BufferGroup(HWND__* window, DirectX12Context* context): m_window(window), m_context(context)
	{
	}
	DirectX12BufferGroup::~DirectX12BufferGroup()
	{
	}
	void DirectX12BufferGroup::Bind() const
	{
		for (auto vBuffer : m_vBuffers) {
			m_context->mCommandList->IASetVertexBuffers(0, 1, &((DirectX12VertexBuffer *)vBuffer.get())->VertexBufferView());
		}
		m_context->mCommandList->IASetIndexBuffer(&((DirectX12IndexBuffer *)m_indexBuffer.get())->IndexBufferView());
	}
	void DirectX12BufferGroup::Unbind() const
	{
	}
	void DirectX12BufferGroup::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		m_vBuffers.push_back(vertexBuffer);
	}
	void DirectX12BufferGroup::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		m_indexBuffer = indexBuffer;
	}
	const std::vector<Ref<VertexBuffer>>& DirectX12BufferGroup::GetVertexBuffers() const
	{
		return m_vBuffers;
	}
	const Ref<IndexBuffer>& DirectX12BufferGroup::GetIndexBuffer() const
	{
		return m_indexBuffer;
	}
}
#endif