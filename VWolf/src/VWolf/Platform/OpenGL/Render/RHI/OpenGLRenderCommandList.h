//
//  OpenGLRenderCommandList.h
//  VWolf
//
//  Created by Enrique Moises on 10/13/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"

#include "VWolf/Core/Render/RHI/CommandList.h"

#include "OpenGLCommandQueue.h"
#include "OpenGLProgram.h"
#include "OpenGLRenderState.h"
#include "OpenGLBuffer.h"
#include "OpenGLBufferSet.h"
#include "OpenGLDriver.h"

namespace VWolf::RHI {
    

    class OpenGLRenderCommandList: public RenderCommandList {
    public:
        OpenGLRenderCommandList(Weak<OpenGLCommandQueue> commandQueue, Weak<RenderPass> renderPass);
        
        virtual Weak<CommandQueue> GetCommandQueue() override;

        virtual void SetRenderState(Ref<RenderState> renderState) override;
        virtual bool SetVertexBuffers(Ref<BufferSet> vertex_buffers) override;
        virtual bool SetIndexBuffer(Ref<Buffer> index_buffer) override;
        virtual void SetProgramBindings(const ProgramArgumentBindings& bindings) override;
        virtual void Commit() override;
        virtual void Draw(RenderPrimitive primitive, uint32_t vertex_count, uint32_t start_vertex = 0,
                          uint32_t instance_count = 1, uint32_t start_instance = 0) override;
        virtual void DrawIndexed(RenderPrimitive primitive, uint32_t index_count = 0, uint32_t start_index = 0, uint32_t start_vertex = 0,
                                 uint32_t instance_count = 1, uint32_t start_instance = 0) override;
    private:
        Weak<OpenGLCommandQueue> commandQueue;
        Weak<RenderPass> renderPass;
        
        Ref<RenderState> current_state;
        Ref<BufferSet> next_vertex_buffers;
        Ref<Buffer> next_index_buffer;
        ProgramArgumentBindings bindings;
    };
}
