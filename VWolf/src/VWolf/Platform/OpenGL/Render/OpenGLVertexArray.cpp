#include "vwpch.h"
#include "OpenGLVertexArray.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {

    static GLenum AttributeFormatToOpenGLBaseType(AttributeFormat type)
    {
        switch (type)
        {
            case AttributeFormat::Float32:
            case AttributeFormat::Float16:
                return GL_FLOAT;
            case AttributeFormat::SInt8:
                return GL_BYTE;
            case AttributeFormat::SInt16:
                return GL_SHORT;
            case AttributeFormat::SInt32:
                return GL_INT;
            case AttributeFormat::UInt8:
                return GL_UNSIGNED_BYTE;
            case AttributeFormat::UInt16:
                return GL_UNSIGNED_SHORT;
            case AttributeFormat::UInt32:
                return GL_UNSIGNED_INT;
            default: return GL_INVALID_VALUE;
        }

        VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray(std::vector<AttributeDescriptor> layout): m_layout(layout) {
        GLThrowIfFailed(glGenVertexArrays(1, &vertexArrayId));
    }

    OpenGLVertexArray::OpenGLVertexArray(const Ref<OpenGLVertexBuffer>& vertexBuffer, std::vector<AttributeDescriptor>  layout): m_layout(layout) {
        GLThrowIfFailed(glGenVertexArrays(1, &vertexArrayId));
        Build(vertexBuffer);
    }

    void OpenGLVertexArray::Build(const Ref<OpenGLVertexBuffer>& vertexBuffer) {
        Bind();
        vertexBuffer->Bind();
        
        size_t offset = 0, stride = 0;
        
        for (auto& element : m_layout) {
            element.offset = (uint32_t)offset;
            size_t size = 0;
            switch (element.GetFormat()) {
                case AttributeFormat::Float32:
                    size = sizeof(float);
                    break;
                case AttributeFormat::SInt32:
                    size = sizeof(uint32_t);
                    break;
                case AttributeFormat::SInt8:
                    size = sizeof(bool);
                    break;
                default: size = 1;
            }
            offset += (size * element.GetDimension());
            stride += (size * element.GetDimension());
        }
        
        for (auto& element : m_layout)
        {
            switch (element.GetFormat()) {
                case AttributeFormat::Float32:
                    GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
                    GLThrowIfFailed(glVertexAttribPointer(m_VertexBufferIndex,
                                                          element.GetDimension(),
                                                          AttributeFormatToOpenGLBaseType(element.GetFormat()),
                                                          GL_FALSE,
                                                          stride,
                                                          (const void*)element.offset));
                    m_VertexBufferIndex++;
                    break;
                case AttributeFormat::SInt32:
                    GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
                    GLThrowIfFailed(glVertexAttribIPointer(m_VertexBufferIndex,
                                                           element.GetDimension(),
                                                           AttributeFormatToOpenGLBaseType(element.GetFormat()),
                                                           stride,
                                                           (const void*)element.offset));
                    m_VertexBufferIndex++;
                    break;
                    break;
                case AttributeFormat::SInt8:
                    
                    break;
                default: break;
            }
        }
        
//        for (auto& element : m_layout)
//        {
//            switch (element.Type)
//            {
//            case ShaderDataType::Float:
//            case ShaderDataType::Float2:
//            case ShaderDataType::Float3:
//            case ShaderDataType::Float4:
//            {
//                GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
//                GLThrowIfFailed(glVertexAttribPointer(m_VertexBufferIndex,
//                                                      element.GetDimension(),
//                                                      ShaderDataTypeToOpenGLBaseType(element.Type),
//                                                      element.Normalized ? GL_TRUE : GL_FALSE,
//                                                      m_layout.GetStride(),
//                                                      (const void*)element.Offset));
//                m_VertexBufferIndex++;
//                break;
//            }
//            case ShaderDataType::Int:
//            case ShaderDataType::Int2:
//            case ShaderDataType::Int3:
//            case ShaderDataType::Int4:
//            case ShaderDataType::Bool:
//            {
//                GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
//                GLThrowIfFailed(glVertexAttribIPointer(m_VertexBufferIndex,
//                                                       element.GetComponentCount(),
//                                                       ShaderDataTypeToOpenGLBaseType(element.Type),
//                                                       m_layout.GetStride(),
//                                                       (const void*)element.Offset));
//                m_VertexBufferIndex++;
//                break;
//            }
//            case ShaderDataType::Mat3:
//            case ShaderDataType::Mat4:
//            {
//                uint8_t count = element.GetComponentCount();
//                for (uint8_t i = 0; i < count; i++)
//                {
//                    GLThrowIfFailed(glEnableVertexAttribArray(m_VertexBufferIndex));
//                    GLThrowIfFailed(glVertexAttribPointer(m_VertexBufferIndex,
//                                                          count,
//                                                          ShaderDataTypeToOpenGLBaseType(element.Type),
//                                                          element.Normalized ? GL_TRUE : GL_FALSE,
//                                                          m_layout.GetStride(),
//                                                          (const void*)(element.Offset + sizeof(float) * count * i)));
//                    GLThrowIfFailed(glVertexAttribDivisor(m_VertexBufferIndex, 1));
//                    m_VertexBufferIndex++;
//                }
//                break;
//            }
//            default:
//                VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!");
//            }
//        }
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
