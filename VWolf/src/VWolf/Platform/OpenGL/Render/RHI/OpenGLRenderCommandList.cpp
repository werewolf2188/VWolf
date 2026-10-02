//
//  OpenGLRenderCommandList.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/13/26.
//

#include "vwpch.h"
#include "OpenGLRenderCommandList.h"
#include "OpenGLTexture.h"

namespace VWolf::RHI {
    namespace {
        static GLenum GetTopology(RenderPrimitive topology) {
            switch (topology) {
                case RenderPrimitive::Triangle:
                    return GL_TRIANGLES;
                case RenderPrimitive::Line:
                    return GL_LINES;
                case RenderPrimitive::LineStrip:
                    return GL_LINE_STRIP;
                case RenderPrimitive::Point:
                    return GL_POINTS;
                default: return -1;
            }
        }
    }

    OpenGLRenderCommandList::OpenGLRenderCommandList(Weak<OpenGLCommandQueue> commandQueue, Weak<RenderPass> renderPass):
    commandQueue(commandQueue), renderPass(renderPass) {
        
    }

    Weak<CommandQueue> OpenGLRenderCommandList::GetCommandQueue() {
        return commandQueue;
    }

    void OpenGLRenderCommandList::SetRenderState(Ref<RenderState> renderState) {
        current_state = renderState;
        DynamicAs<OpenGLProgram>(current_state->GetSettings().GetProgram())->Bind();
    }

    bool OpenGLRenderCommandList::SetVertexBuffers(Ref<BufferSet> vertex_buffers) {
        next_vertex_buffers = vertex_buffers;
        return true;
    }

    bool OpenGLRenderCommandList::SetIndexBuffer(Ref<Buffer> index_buffer) {
        next_index_buffer = index_buffer;
        return true;
    }

    void OpenGLRenderCommandList::SetProgramBindings(const ProgramArgumentBindings& bindings) {
        this->bindings = bindings;
    }

    void OpenGLRenderCommandList::Commit() {
        DynamicAs<OpenGLProgram>(current_state->GetSettings().GetProgram())->Unbind();
    }

    void OpenGLRenderCommandList::Draw(RenderPrimitive primitive, uint32_t vertex_count, uint32_t start_vertex,
                                       uint32_t instance_count, uint32_t start_instance) {
        
    }
    void OpenGLRenderCommandList::DrawIndexed(RenderPrimitive primitive, uint32_t index_count, uint32_t start_index, uint32_t start_vertex,
                                              uint32_t instance_count, uint32_t start_instance) {
        // 1. Look for the group.
        std::string name = next_vertex_buffers->GetName();
        boost::algorithm::replace_all(name, "vertexSet", "vertexArray");
        Ref<OpenGLVertexArray> vertexArray;
        if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(name)) {
            vertexArray = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLVertexArray>(name);
        } else {
            DynamicAs<OpenGLBufferSet>(next_vertex_buffers)->ApplySubResource();
            vertexArray = DynamicAs<OpenGLProgram>(current_state->GetSettings().GetProgram())->CreateVertexArray(
                                                                                                                 DynamicAs<OpenGLBufferSet>(next_vertex_buffers),
                                                                                                                 DynamicAs<OpenGLBuffer>(next_index_buffer));
            vertexArray->Bind();
            
            DynamicAs<OpenGLBuffer>(next_index_buffer)->ApplySubResource();
            vertexArray->Unbind();
            vertexArray->SetName(name);
            OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(vertexArray);
        }
        
        // 2. Bind the programs arguments
        uint32_t textureIndex = 0;
        for(const auto& [key, value]: bindings.GetParameters()) {
            if (key->GetResourceType() == ResourceType::Buffer) {
                DynamicAs<OpenGLProgram>(current_state->GetSettings().GetProgram())->ReplaceProgramBinding(key, value);
                const ResourceView& view = std::get<ResourceView>(value);
                Ref<OpenGLBuffer> buffer = DynamicAs<OpenGLBuffer>(view.GetResource());
                buffer->Bind();
            } else if (key->GetResourceType() == ResourceType::Texture) {
                const TextureView& view = std::get<TextureView>(value);
                Ref<OpenGLTexture> texture = DynamicAs<OpenGLTexture>(view.GetTexture().lock());
                texture->Bind(textureIndex);
                textureIndex++;
            }
        }
        
        // 3. Bind the vertex array
        vertexArray->Bind();
        vertexArray->GetVertexSet()->Bind();
        vertexArray->GetIndexBuffer()->Bind();
        
        // 4. Draw
        GLThrowIfFailed(glDrawElements(GetTopology(primitive), index_count, GL_UNSIGNED_INT, nullptr));
        
        textureIndex = 0;
        for(const auto& [key, value]: bindings.GetParameters()) {
            if (key->GetResourceType() == ResourceType::Buffer) {
                DynamicAs<OpenGLProgram>(current_state->GetSettings().GetProgram())->ReplaceProgramBinding(key, value);
                const ResourceView& view = std::get<ResourceView>(value);
                Ref<OpenGLBuffer> buffer = DynamicAs<OpenGLBuffer>(view.GetResource());
                buffer->Unbind();
            } else if (key->GetResourceType() == ResourceType::Texture) {
                const TextureView& view = std::get<TextureView>(value);
                Ref<OpenGLTexture> texture = DynamicAs<OpenGLTexture>(view.GetTexture().lock());
                texture->Unbind(textureIndex);
                textureIndex++;
            }
        }
        
        // 5. Unbind
        vertexArray->GetVertexSet()->Unbind();
        vertexArray->GetIndexBuffer()->Unbind();
        vertexArray->Unbind();
    }
}
