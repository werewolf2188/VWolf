#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12BufferGroup.h"

#include "DirectX12Buffer.h"

#include "VWolf/Platform/DirectX12/Core/DX12Command.h"

namespace VWolf {
	DirectX12BufferGroup::DirectX12BufferGroup()
	{
	}

	DirectX12BufferGroup::~DirectX12BufferGroup()
	{
	}

	void DirectX12BufferGroup::Bind(Ref<DX12Command> commands) const
	{
		m_vBuffer->Bind(commands);
		m_indexBuffer->Bind(commands);
	}

	void DirectX12BufferGroup::SetVertexBuffer(const Ref<DirectX12VertexBuffer>& vertexBuffer)
	{
		m_vBuffer = vertexBuffer;
	}
	void DirectX12BufferGroup::SetIndexBuffer(const Ref<DirectX12IndexBuffer>& indexBuffer)
	{
		m_indexBuffer = indexBuffer;
	}
}
#endif