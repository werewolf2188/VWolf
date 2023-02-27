#include "vwpch.h"
#include "OpenGLVertexArray.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

    OpenGLVertexArray::OpenGLVertexArray(): m_layout(MeshData::Layout) {
        GLThrowIfFailed(glGenVertexArrays(1, &vertexArrayId));
    }

	OpenGLVertexArray::OpenGLVertexArray(const Ref<OpenGLVertexBuffer>& vertexBuffer): OpenGLVertexArray(vertexBuffer,
                                                                                                         MeshData::Layout) { }

    OpenGLVertexArray::OpenGLVertexArray(const Ref<OpenGLVertexBuffer>& vertexBuffer, const BufferLayout& layout): m_layout(layout) {
        GLThrowIfFailed(glGenVertexArrays(1, &vertexArrayId));
        Build(vertexBuffer);
    }

    void OpenGLVertexArray::Build(const Ref<OpenGLVertexBuffer>& vertexBuffer) {
        Bind();
        vertexBuffer->Bind();
        for (const auto& element : m_layout)
        {
            switch (element.Type)
            {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            {
                GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLThrowIfFailed(glVertexAttribPointer(m_VertexBufferIndex,
                                                      element.GetComponentCount(),
                                                      ShaderDataTypeToOpenGLBaseType(element.Type),
                                                      element.Normalized ? GL_TRUE : GL_FALSE,
                                                      m_layout.GetStride(),
                                                      (const void*)element.Offset));
                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLThrowIfFailed(glVertexAttribIPointer(m_VertexBufferIndex,
                                                       element.GetComponentCount(),
                                                       ShaderDataTypeToOpenGLBaseType(element.Type),
                                                       m_layout.GetStride(),
                                                       (const void*)element.Offset));
                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
                    GLThrowIfFailed(glVertexAttribPointer(m_VertexBufferIndex,
                                                          count,
                                                          ShaderDataTypeToOpenGLBaseType(element.Type),
                                                          element.Normalized ? GL_TRUE : GL_FALSE,
                                                          m_layout.GetStride(),
                                                          (const void*)(element.Offset + sizeof(float) * count * i)));
                    GLThrowIfFailed(glVertexAttribDivisor(m_VertexBufferIndex, 1));
                    m_VertexBufferIndex++;
                }
                break;
            }
            default:
                VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!");
            }
        }
        vertexBuffer->Unbind();
        Unbind();
    }

	OpenGLVertexArray::~OpenGLVertexArray()
	{
        GLThrowIfFailed(glDeleteVertexArrays(1, &vertexArrayId));
	}

	void OpenGLVertexArray::Bind() const
	{
        GLThrowIfFailed(glBindVertexArray(vertexArrayId));
	}
	void OpenGLVertexArray::Unbind() const
	{
        GLThrowIfFailed(glBindVertexArray(0));
	}
}
