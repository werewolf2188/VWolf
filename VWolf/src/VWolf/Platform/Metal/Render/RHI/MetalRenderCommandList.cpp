//
//  MetalRenderCommandList.cpp
//  VWolf
//
//  Created by Enrique Moises on 8/18/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalRenderCommandList.h"
#include "MetalRenderPass.h"
#include "MetalDriver.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

#define IR_RUNTIME_METALCPP
#include <metal_irconverter_runtime/metal_irconverter_runtime.h>

namespace VWolf::RHI  {
    static MTL::PrimitiveType GetTopology(RenderPrimitive topology) {
        switch (topology) {
            case RenderPrimitive::Triangle:
                return MTL::PrimitiveType::PrimitiveTypeTriangle;
            case RenderPrimitive::TriangleStrip:
                return MTL::PrimitiveType::PrimitiveTypeTriangleStrip;
            case RenderPrimitive::Line:
                return MTL::PrimitiveType::PrimitiveTypeLine;
            case RenderPrimitive::LineStrip:
                return MTL::PrimitiveType::PrimitiveTypeLineStrip;
            case RenderPrimitive::Point:
                return MTL::PrimitiveType::PrimitiveTypePoint;
            default: return MTL::PrimitiveType::PrimitiveTypePoint;
        }
    }

    MetalRenderCommandList::MetalRenderCommandList(Weak<MetalCommandQueue> commandQueue, Weak<RenderPass> renderPass):
    commandQueue(commandQueue), renderPass(renderPass) {
        if (Ref<MetalCommandQueue> cq = commandQueue.lock()) {
            commandBuffer = UnownedRef<MTL::CommandBuffer>(cq->GetCommandQueue()->commandBuffer());
            if (Ref<RenderPass> rp = renderPass.lock()) {
                Ref<MetalRenderPass> mrp = DynamicAs<MetalRenderPass>(rp);
                encoder = UnownedRef<MTL::RenderCommandEncoder>(commandBuffer->renderCommandEncoder(mrp->GetRenderPassDescriptor().get()));
            }
        }
    }

    Weak<CommandQueue> MetalRenderCommandList::GetCommandQueue() {
        return commandQueue;
    }

    void MetalRenderCommandList::SetName(const std::string& name) {
        Object::SetName(name);
        if (!name.empty()) {
            std::string encoderName = name + " - encoder";
            std::string commandBufferName = name + " - command buffer";
            encoder->setLabel(NSSTRINGFROM(encoderName.data()));
            commandBuffer->setLabel(NSSTRINGFROM(commandBufferName.data()));
        }
    }

    void MetalRenderCommandList::SetRenderState(Ref<RenderState> renderState) {
        currentStateName = renderState->GetSettings().GetProgram()->GetSettings().GetShaders()[0]->GetSettings().GetShaderName();
        current_state = renderState;
        
        Ref<MetalRenderState> metalRenderState = DynamicAs<MetalRenderState>(renderState);
        encoder->setRenderPipelineState(metalRenderState->GetState().get());
        
        SetRasterization(metalRenderState);
        SetDepthStencil(metalRenderState);
    }
    bool MetalRenderCommandList::SetVertexBuffers(Ref<BufferSet> vertex_buffers) {
        next_vertex_buffers = vertex_buffers;
        Ref<MetalBuffer> metal_index_vertex_buffer = DynamicAs<MetalBuffer>(next_vertex_buffers->GetBuffers()[0]);
        encoder->setVertexBuffer(metal_index_vertex_buffer->GetBuffer().get(), 0, kIRVertexBufferBindPoint);
        return true;
    }

    bool MetalRenderCommandList::SetIndexBuffer(Ref<Buffer> index_buffer) {
        next_index_buffer = index_buffer;
        return true;
    }

    void MetalRenderCommandList::SetProgramBindings(const ProgramArgumentBindings& bindings) {
        CreateDescriptorTableEntries(GetCurrentIndexPerState());
        SetDescriptorTable(GetCurrentIndexPerState(), ShaderType::Vertex);
        SetDescriptorTable(GetCurrentIndexPerState(), ShaderType::Fragment);
        
        for(const auto& [key, value]: bindings.GetParameters()) {
            if (key->GetResourceType() == ResourceType::Buffer) {
                const ResourceView& view = std::get<ResourceView>(value);
                Ref<MetalBuffer> buffer = DynamicAs<MetalBuffer>(view.GetResource());
                SetData(buffer->GetBuffer(), key->GetName(), GetCurrentIndexPerState(), buffer->GetSettings().GetSize());
            } else if (key->GetResourceType() == ResourceType::Texture) {
                const TextureView& view = std::get<TextureView>(value);
                Ref<MetalTexture> texture = DynamicAs<MetalTexture>(view.GetTexture().lock());
                encoder->useResource(texture->GetTexture().get(), MTL::ResourceUsageRead, MTL::RenderStageFragment);
                SetTexture(texture->GetTexture(), key->GetName(), GetCurrentIndexPerState());
            }
        }
    }

    void MetalRenderCommandList::Commit() {
        encoder->endEncoding();
        commandBuffer->commit();
        
        elementIndex = 0;
        currentStateName = "";
        itemsPerRenderState.clear();
    }

    void MetalRenderCommandList::Draw(RenderPrimitive primitive, uint32_t vertex_count, uint32_t start_vertex,
                                      uint32_t instance_count, uint32_t start_instance) {
        
        IRRuntimeDrawPrimitives(encoder.get(), GetTopology(primitive), start_vertex, 1, instance_count, start_instance);
        elementIndex++;
        IncrementCurrentIndexPerState();
    }

    void MetalRenderCommandList::DrawIndexed(RenderPrimitive primitive, uint32_t index_count, uint32_t start_index, uint32_t start_vertex,
                     uint32_t instance_count, uint32_t start_instance) {
        
        MTL::IndexType indexType = next_index_buffer->GetSettings().GetFormat() == RHI::PixelFormat::R32Uint ? MTL::IndexTypeUInt32 : MTL::IndexTypeUInt16;
        Ref<MetalBuffer> metal_index_vertex_buffer = DynamicAs<MetalBuffer>(next_index_buffer);
        
        IRRuntimeDrawIndexedPrimitives(encoder.get(), GetTopology(primitive), index_count, indexType, metal_index_vertex_buffer->GetBuffer().get(), 0);
        elementIndex++;
        IncrementCurrentIndexPerState();
    }

    void MetalRenderCommandList::SetRasterization(Ref<MetalRenderState> renderState) {
        bool cullEnabled = renderState->GetSettings().GetRasterizerSettings().GetCullEnabled();
        RasterizerCullMode cullMode = renderState->GetSettings().GetRasterizerSettings().GetCullMode();
        RasterizerFillMode fillMode = renderState->GetSettings().GetRasterizerSettings().GetFillMode();
    //        bool counterClockwise = settings.GetRasterization().GetCounterClockwise();
        
        if (cullEnabled) {
            switch (cullMode) {
                case RasterizerCullMode::Front:
                    encoder->setCullMode(MTL::CullMode::CullModeFront);
                    break;
                case RasterizerCullMode::Back:
                    encoder->setCullMode(MTL::CullMode::CullModeBack);
                    break;
                case RasterizerCullMode::FrontAndBack:
                    encoder->setCullMode(MTL::CullMode::CullModeNone);
                    break;
            }
        } else
            encoder->setCullMode(MTL::CullMode::CullModeNone);
        switch (fillMode) {
            case RasterizerFillMode::Solid:
                encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeFill);
                break;
            case RasterizerFillMode::Wireframe:
                encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeLines);
                break;
        }
    }

    void MetalRenderCommandList::SetDepthStencil(Ref<MetalRenderState> renderState) {
        if (renderState->GetDepthStencilState() == nullptr) return;
        encoder->setDepthStencilState(renderState->GetDepthStencilState().get());
    }

    uint32_t MetalRenderCommandList::GetCurrentIndexPerState() {
        
        if (itemsPerRenderState.find(currentStateName) == itemsPerRenderState.end()) {
            return 0;
        }
        
        return itemsPerRenderState[currentStateName];
    }

    void MetalRenderCommandList::IncrementCurrentIndexPerState() {
        uint32_t index = GetCurrentIndexPerState();
        itemsPerRenderState[currentStateName] = ++index;
    }

    // TODO: Refactor
    void MetalRenderCommandList::CreateDescriptorTableEntries(uint32_t objectIndex) {
        Ref<MetalProgram> program = DynamicAs<MetalProgram>(current_state->GetSettings().GetProgram());
        
        for(auto& [key, value]: program->GetTopLevelArguments()) {
            ShaderType shaderType = (ShaderType)(uint32_t)key;
            if (program->pDescriptorTables[shaderType].size() <= objectIndex) {
                const uint32_t kNumEntries = (uint32_t)value.size();
                size_t size= sizeof(IRDescriptorTableEntry) * kNumEntries;
                
                if (size == 0) continue;
                
                MTL::Buffer* pDescriptorTable = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(size, MTL::ResourceStorageModeShared);
                auto argumentBufferPtr = static_cast<uint8_t*>(pDescriptorTable->contents());
                
                std::map<std::string, Ref<RHI::BindableProgramArgument>> programArguments;
                std::copy_if(program->GetSettings().GetProgramArguments().begin(), program->GetSettings().GetProgramArguments().end(),
                                 std::inserter(programArguments, programArguments.end()),
                                 [](const auto& pair) {
                                     return pair.second->GetResourceType() == RHI::ResourceType::Sampler;
                                 });
                
                for(auto [skey, svalue]: programArguments) {
                    if (value.count(skey) == 0) continue;
                    
                    const Ref<RHI::Argument>& resource = value.at(svalue->GetName());
                    uint64_t _offset = resource->GetOffset();
                    auto entry = reinterpret_cast<IRDescriptorTableEntry*>(argumentBufferPtr + _offset);
                    
                    MTL::SamplerDescriptor* descriptor = MTL::SamplerDescriptor::alloc()->init();
                    descriptor->setMipFilter(MTL::SamplerMipFilter::SamplerMipFilterLinear);
                    descriptor->setRAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
                    descriptor->setTAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
                    descriptor->setSAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
                    descriptor->setMaxAnisotropy(1);
                    descriptor->setCompareFunction(MTL::CompareFunction::CompareFunctionNever);
                    descriptor->setLodMinClamp(0);
                    descriptor->setLodMaxClamp(std::numeric_limits<float>::max());
                    descriptor->setSupportArgumentBuffers(true);
                    IRDescriptorTableSetSampler(entry, MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newSamplerState(descriptor), 0);
                    descriptor->release();
                }
                program->pDescriptorTables[shaderType].push_back(pDescriptorTable);
            }
        }
    }

    void MetalRenderCommandList::SetDescriptorTable(uint32_t index, ShaderType type) {
        Ref<MetalProgram> program = DynamicAs<MetalProgram>(current_state->GetSettings().GetProgram());
        auto& pDescriptorTable = program->pDescriptorTables[type];
        if (pDescriptorTable.size() > index) {
            switch (type) {
                case ShaderType::Vertex:
                    encoder->setVertexBuffer(pDescriptorTable[index], 0, kIRArgumentBufferBindPoint);
                    break;
                case ShaderType::Fragment:
                    encoder->setFragmentBuffer(pDescriptorTable[index], 0, kIRArgumentBufferBindPoint);
                    break;
                default: break;
            }
        }
    }

    void MetalRenderCommandList::SetData(Ref<MTL::Buffer> passArgument, const std::string& name, uint32_t index, uint32_t size) {
        if (size == 0) return;
        
        Ref<MetalProgram> program = DynamicAs<MetalProgram>(current_state->GetSettings().GetProgram());
        
        for(auto& [key, topLevelArgument]: program->GetTopLevelArguments()) {
            if (topLevelArgument.count(name) == 0) continue;
            
            auto argumentBufferPtr = static_cast<uint8_t*>(program->pDescriptorTables[(ShaderType)(uint32_t)key][index]->contents());
            const Ref<RHI::Argument>& resource = topLevelArgument.at(name);
            uint64_t _offset = resource->GetOffset();
            auto entry = reinterpret_cast<IRDescriptorTableEntry*>(argumentBufferPtr + _offset);
            // TODO: variableBuffers is no longer necessary. Context should be aware of its objects that has created.
            // TODO: I should be improving the render context to be aware of what objects it creates based on components or meshes.
//            auto& variableBuffer = program->variableBuffers[(ShaderType)(uint32_t)key];
            
//            MTL::Buffer *passArgument;
//            if (variableBuffer.count(name) == 0) {
//                std::vector<MTL::Buffer*> passArgumentsPerObject;
////                passArgument = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(data, size, MTL::ResourceStorageModeShared);
//                passArgumentsPerObject.push_back(passArgument.get());
//                variableBuffer[name] = passArgumentsPerObject;
//                encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageVertex);
//                encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageFragment);
//            } else {
//                std::vector<MTL::Buffer*>& passArgumentsPerObject = variableBuffer[name];
//                if (index >= passArgumentsPerObject.size()) {
////                    passArgument = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(data, size, MTL::ResourceStorageModeShared);
//                    passArgumentsPerObject.push_back(passArgument.get());
//                    encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageVertex);
//                    encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageFragment);
//                } else {
////                    MTL::Buffer *passArgument = passArgumentsPerObject[index];
////                    memcpy(passArgument->contents(), data, size);
//                    encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageVertex);
//                    encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageFragment);
//                }
//            }
            encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageVertex);
            encoder->useResource(passArgument.get(), MTL::ResourceUsageRead, MTL::RenderStageFragment);
            
            uint64_t gpuAddress = passArgument->gpuAddress();
            
            IRDescriptorTableSetBuffer(entry, gpuAddress, 0);
        }
    }

    void MetalRenderCommandList::SetTexture(Ref<MTL::Texture> texture, const std::string& name, uint32_t index) {
        Ref<MetalProgram> program = DynamicAs<MetalProgram>(current_state->GetSettings().GetProgram());
        
        for(auto& [key, topLevelArgument]: program->GetTopLevelArguments()) {
            if (topLevelArgument.count(name) == 0) continue;
            
            auto argumentBufferPtr = static_cast<uint8_t*>(program->pDescriptorTables[(ShaderType)(uint32_t)key][index]->contents());
            const Ref<RHI::Argument>& resource = topLevelArgument.at(name);
            uint64_t _offset = resource->GetOffset();
            auto entry = reinterpret_cast<IRDescriptorTableEntry*>(argumentBufferPtr + _offset);

            IRDescriptorTableSetTexture(entry, texture.get(), 0, 0);
        }
    }
    //
}

#endif
