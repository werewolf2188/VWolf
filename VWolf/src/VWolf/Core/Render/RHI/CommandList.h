//
//  CommandList.h
//  VWolf
//
//  Created by Enrique Moises on 8/17/26.
//

#pragma once
#include "Object.h"

namespace VWolf::RHI  {
    class CommandQueue;
    class RenderState;
    class Buffer;
    class BufferSet;
    class ProgramArgumentBindings;

    enum class RenderPrimitive
    {
        Point,
        Line,
        LineStrip,
        Triangle,
        TriangleStrip
    };

    class CommandList: public Object {
    public:
        virtual Weak<CommandQueue> GetCommandQueue() = 0;
        
        virtual void SetProgramBindings(const ProgramArgumentBindings& bindings) = 0;
        virtual void Commit() = 0;
    protected:
    private:
    };

    class RenderCommandList: public CommandList {
    public:
        virtual void SetRenderState(Ref<RenderState> renderState) = 0;
        virtual bool SetVertexBuffers(Ref<BufferSet> vertex_buffers) = 0;
        virtual bool SetIndexBuffer(Ref<Buffer> index_buffer) = 0;
        virtual void Draw(RenderPrimitive primitive, uint32_t vertex_count, uint32_t start_vertex = 0,
                          uint32_t instance_count = 1, uint32_t start_instance = 0) = 0;
        virtual void DrawIndexed(RenderPrimitive primitive, uint32_t index_count = 0, uint32_t start_index = 0, uint32_t start_vertex = 0,
                                 uint32_t instance_count = 1, uint32_t start_instance = 0) = 0;
    protected:
    private:
    };
}
