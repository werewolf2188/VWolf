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
        this->subResource = subResource;
    }

    void OpenGLBuffer::ApplySubResource() {
        Bind();
        
        void* ptr = glMapBufferRange(
                                     GetType(),
                                     0,
                                     settings.GetSize(),
                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
                                     );
        GLThrowIfFailedNoAction("glMapBufferRange");
        
        if (ptr) {
            // Copy data from your CPU array (newVertexData) directly into GPU memory
            memcpy(ptr, subResource.GetBytes(), settings.GetSize());
            
            // UNMAP the buffer immediately after copying.
            // This releases the pointer and tells the GPU the data is ready.
            glUnmapBuffer(GetType());
            GLThrowIfFailedNoAction("glUnmapBuffer");
        }
        
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
                return GL_ELEMENT_ARRAY_BUFFER;
            case BufferType::Constant:
                return GL_UNIFORM_BUFFER;
        }
    }
}
