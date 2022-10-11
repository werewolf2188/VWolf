#pragma once
#include "VWolf/Core/Base.h"
#include "RenderStructs.h"
#include <functional>
#ifdef VWOLF_PLATFORM_MACOS
#include <vector>
#endif

namespace VWolf {
	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t size) : m_size(size) {};
		VertexBuffer(float* vertices, uint32_t size) : m_vertices(vertices), m_size(size) {};
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

#ifdef VWOLF_CORE
		static void SetDefaultCreateSizeMethod(std::function<Ref<VertexBuffer>(uint32_t size)> createSize) { m_createSize = createSize; }
		static void SetDefaultCreateDataAndSizeMethod(std::function<Ref<VertexBuffer>(float* vertices, uint32_t size)> createDataAndSize) { m_createDataAndSize = createDataAndSize; }
#endif
		static Ref<VertexBuffer> Create(uint32_t size) {
			return m_createSize(size);
		}
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size) { 
			return m_createDataAndSize(vertices, size);
		}
	protected:
		float* m_vertices;
		uint32_t m_size;
	private:
		static std::function<Ref<VertexBuffer>(uint32_t size)> m_createSize;
		static std::function<Ref<VertexBuffer>(float* vertices, uint32_t size)> m_createDataAndSize;
	};

	inline std::function<Ref<VertexBuffer>(uint32_t size)> VertexBuffer::m_createSize = nullptr;
	inline std::function<Ref<VertexBuffer>(float* vertices, uint32_t size)> VertexBuffer::m_createDataAndSize = nullptr;

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* indices, uint32_t count) : m_indices(indices), m_count(count) {};
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

#ifdef VWOLF_CORE
		static void SetDefaultCreateMethod(std::function<Ref<IndexBuffer>(uint32_t* indices, uint32_t count)> create) { m_create = create; }
#endif
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count) {
			return m_create(indices, count);
		}
	protected: 
		uint32_t* m_indices;
		uint32_t m_count;
	private:
		static std::function<Ref<IndexBuffer>(uint32_t* indices, uint32_t count)> m_create;
	};

	inline std::function<Ref<IndexBuffer>(uint32_t* indices, uint32_t count)> IndexBuffer::m_create;
}