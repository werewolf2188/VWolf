//
//  MetalRenderCommandList.h
//  VWolf
//
//  Created by Enrique Moises on 8/18/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/CommandList.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalCommandQueue.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalProgram.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderState.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalBuffer.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalBufferSet.h"

namespace VWolf::RHI  {
    class MetalRenderCommandList: public RenderCommandList {
    public:
        MetalRenderCommandList(Weak<MetalCommandQueue> commandQueue, Weak<RenderPass> renderPass);
        
        virtual Weak<CommandQueue> GetCommandQueue() override;
        virtual void SetName(const std::string& name) override;
        virtual void SetRenderState(Ref<RenderState> renderState) override;
        virtual bool SetVertexBuffers(Ref<BufferSet> vertex_buffers) override;
        virtual bool SetIndexBuffer(Ref<Buffer> index_buffer) override;
        virtual void SetProgramBindings(const ProgramArgumentBindings& bindings) override;
        virtual void Commit() override;
        virtual void Draw(RenderPrimitive primitive, uint32_t vertex_count, uint32_t start_vertex = 0,
                          uint32_t instance_count = 1, uint32_t start_instance = 0) override;
        virtual void DrawIndexed(RenderPrimitive primitive, uint32_t index_count = 0, uint32_t start_index = 0, uint32_t start_vertex = 0,
                                 uint32_t instance_count = 1, uint32_t start_instance = 0) override;
        
        Ref<MTL::CommandBuffer> GetCommandBuffer() { return commandBuffer; }
        Ref<MTL::RenderCommandEncoder> GetEncoder() { return encoder; }
    protected:
    private:
        void SetRasterization(Ref<MetalRenderState> renderState);
        void SetDepthStencil(Ref<MetalRenderState> renderState);
        
        uint32_t GetCurrentIndexPerState();
        void IncrementCurrentIndexPerState();
        // TODO: Refactor
        void CreateDescriptorTableEntries(uint32_t objectIndex);
        void SetDescriptorTable(uint32_t index, ShaderType type);
        void SetData(Ref<RHI::MetalBuffer>& buffer, const Ref<BindableProgramArgument>& argument, uint32_t index, uint32_t size);
        void SetTexture(Ref<MTL::Texture> texture, const std::string& name, uint32_t index);
        void SetSampler(Ref<MTL::SamplerState> sampler, const std::string& name, uint32_t index);
        //
        
        Ref<MTL::CommandBuffer> commandBuffer;
        Ref<MTL::RenderCommandEncoder> encoder;
        
        Weak<MetalCommandQueue> commandQueue;
        Weak<RenderPass> renderPass;
        
        Ref<RenderState> current_state;
        Ref<BufferSet> next_vertex_buffers;
        Ref<Buffer> next_index_buffer;
        
        uint32_t elementIndex = 0;
        std::string currentStateName = "";
        std::map<std::string, uint32_t> itemsPerRenderState;
    };
}
