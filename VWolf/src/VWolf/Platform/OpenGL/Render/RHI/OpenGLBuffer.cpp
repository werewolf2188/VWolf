//
//  OpenGLBuffer.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/13/26.
//

#include "vwpch.h"
#include "OpenGLBuffer.h"

namespace VWolf::RHI {
    OpenGLBuffer::OpenGLBuffer(Weak<OpenGLRenderContext> context, const BufferSettings& settings):
    Buffer(settings) {
        GLThrowIfFailed(glGenBuffers(1, &bufferId));
        Bind();
        GLThrowIfFailed(glBufferData(GetType(), settings.GetSize(), nullptr, GL_DYNAMIC_DRAW));
        Unbind();
    }

    OpenGLBuffer::~OpenGLBuffer() {
        GLThrowIfFailed(glDeleteBuffers(1, &bufferId));
    }

    void OpenGLBuffer::SetData(const SubResource& subResource) {
        Bind();

        GLThrowIfFailed(glBufferSubData(GetType(), 0, settings.GetSize(), subResource.GetBytes()));

        Unbind();
    }

    ResourceView OpenGLBuffer::GetResourceView() {
        return ResourceView(shared_from_this(), {});
    }

    void OpenGLBuffer::Bind() {
        GLThrowIfFailed(glBindBuffer(GetType(), bufferId));
    }

    void OpenGLBuffer::Unbind(){
        GLThrowIfFailed(glBindBuffer(GetType(), 0));
    }

    GLuint OpenGLBuffer::GetType() {
        switch (settings.GetType()) {
            case BufferType::Vertex:
                return GL_ARRAY_BUFFER;
            case BufferType::Index:
                return isIndexBufferAttachedToVertexArray ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER ;
            case BufferType::Constant:
                return GL_UNIFORM_BUFFER;
        }
    }
}
