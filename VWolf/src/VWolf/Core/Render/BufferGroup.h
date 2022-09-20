#pragma once

#include "VWolf/Core/Base.h"
#include "Buffer.h"

#include <vector>

namespace VWolf {
	class BufferGroup
	{
	public:
		virtual ~BufferGroup() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

#ifdef VWOLF_CORE
		static void SetDefaultCreateMethod(std::function<Ref<BufferGroup>()> create) { m_create = create; }
#endif
		static Ref<BufferGroup> Create() {
			return m_create();
		};
	private:
		static std::function<Ref<BufferGroup>()> m_create;
	};

	inline std::function<Ref<BufferGroup>()> BufferGroup::m_create = nullptr;
}