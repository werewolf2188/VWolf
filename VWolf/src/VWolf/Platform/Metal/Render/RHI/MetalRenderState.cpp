//
//  MetalRenderState.cpp
//  VWolf
//
//  Created by Enrique Moises on 9/2/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalRenderState.h"
#include "MetalProgram.h"
#include "MetalDriver.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

#define IR_RUNTIME_METALCPP
#define IR_PRIVATE_IMPLEMENTATION
#include <metal_irconverter_runtime/metal_irconverter_runtime.h>

namespace VWolf::RHI {

    MTL::BlendOperation GetOperation(BlendEquation equation) {
        switch (equation) {
            case BlendEquation::Add:
                return MTL::BlendOperation::BlendOperationAdd;
            case BlendEquation::Substract:
                return MTL::BlendOperation::BlendOperationSubtract;
            case BlendEquation::ReverseSubstract:
                return MTL::BlendOperation::BlendOperationReverseSubtract;
            case BlendEquation::Min:
                return MTL::BlendOperation::BlendOperationMin;
            case BlendEquation::Max:
                return MTL::BlendOperation::BlendOperationMax;
        }
    }

    MTL::BlendFactor GetFactor(BlendFunction function) {
        switch (function) {
            case BlendFunction::Zero:
                return MTL::BlendFactor::BlendFactorZero;
            case BlendFunction::One:
                return MTL::BlendFactor::BlendFactorOne;
            case BlendFunction::SrcColor:
                return MTL::BlendFactor::BlendFactorSourceColor;
            case BlendFunction::InvSrcColor:
                return MTL::BlendFactor::BlendFactorOneMinusSourceColor;
            case BlendFunction::DstColor:
                return MTL::BlendFactor::BlendFactorDestinationColor;
            case BlendFunction::InvDstColor:
                return MTL::BlendFactor::BlendFactorOneMinusDestinationColor;
            case BlendFunction::SrcAlpha:
                return MTL::BlendFactor::BlendFactorSourceAlpha;
            case BlendFunction::InvSrcAlpha:
                return MTL::BlendFactor::BlendFactorOneMinusSourceAlpha;
            case BlendFunction::DstAlpha:
                return MTL::BlendFactor::BlendFactorDestinationAlpha;
            case BlendFunction::InvDstAlpha:
                return MTL::BlendFactor::BlendFactorOneMinusDestinationAlpha;
            case BlendFunction::Src1Color:
                return MTL::BlendFactor::BlendFactorSource1Color;
            case BlendFunction::InvSrc1Color:
                return MTL::BlendFactor::BlendFactorOneMinusSource1Color;
            case BlendFunction::Src1Alpha:
                return MTL::BlendFactor::BlendFactorSource1Alpha;
            case BlendFunction::InvSrc1Alpha:
                return MTL::BlendFactor::BlendFactorOneMinusSource1Alpha;
            case BlendFunction::SrcAlphaSat:
                return MTL::BlendFactor::BlendFactorSourceAlphaSaturated;
            case BlendFunction::CnstColor:
                return MTL::BlendFactor::BlendFactorBlendColor;
            case BlendFunction::InvCnstColor:
                return MTL::BlendFactor::BlendFactorOneMinusBlendColor;
            case BlendFunction::CnstAlpha:
                return MTL::BlendFactor::BlendFactorBlendAlpha;
            case BlendFunction::InvCnstAlpha:
                return MTL::BlendFactor::BlendFactorOneMinusBlendAlpha;
        }
    }

    MetalRenderState::MetalRenderState(Weak<RenderContext> context, const RenderStateSettings& settings):
    RenderState(context, settings) {
        ReflectLibraryAndCreateState();
        PrepareDepthStencilState();
    }

    MetalRenderState::~MetalRenderState() {}

    void MetalRenderState::ReflectLibraryAndCreateState() {
        MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        Ref<RHI::MetalProgram> program = DynamicAs<RHI::MetalProgram>(settings.GetProgram());
        for(auto kv: program->GetFunctions()) {
            Ref<MTL::Function> function = kv.second;
            switch(function->functionType()) {
                case MTL::FunctionType::FunctionTypeVertex:
                    descriptor->setVertexFunction(function.get());
                    break;
                case MTL::FunctionTypeFragment:
                    descriptor->setFragmentFunction(function.get());
                    break;
                default: break;
            }
        }

        descriptor->colorAttachments()->object(0)->setPixelFormat(RHI::GetFormatFrom(MetalDriver::GetCurrent()->GetRenderContext()->GetSettings().GetColorFormat()));
        PrepareBlending(descriptor->colorAttachments()->object(0));
        descriptor->setDepthAttachmentPixelFormat(RHI::GetFormatFrom(MetalDriver::GetCurrent()->GetRenderContext()->GetSettings().GetDepthStencilFormat()));
        descriptor->setStencilAttachmentPixelFormat(RHI::GetFormatFrom(MetalDriver::GetCurrent()->GetRenderContext()->GetSettings().GetDepthStencilFormat()));

        NS::UInteger stride = 0;
        MTL::VertexDescriptor* vertexDescriptor =  MTL::VertexDescriptor::alloc()->init();
        
        for (const Ref<RHI::StageInAttribute>& vertexAttribute: program->GetStageInAttributes()) {
            vertexDescriptor->attributes()->object(kIRStageInAttributeStartIndex + vertexAttribute->GetIndex())->setOffset(stride);
            vertexDescriptor->attributes()->object(kIRStageInAttributeStartIndex + vertexAttribute->GetIndex())->setFormat(vertexAttribute->GetVertexFormat());
            vertexDescriptor->attributes()->object(kIRStageInAttributeStartIndex + vertexAttribute->GetIndex())->setBufferIndex(kIRVertexBufferBindPoint);
            stride += vertexAttribute->GetSize();
        }

        vertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStride(stride);
        vertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStepRate(1);
        vertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStepFunction(MTL::VertexStepFunction::VertexStepFunctionPerVertex);

        descriptor->setVertexDescriptor(vertexDescriptor);

        NS::Error* error = nullptr;
        MTL::RenderPipelineReflection *reflection = nullptr;
        MTL::PipelineOption options = MTL::PipelineOptionArgumentInfo | MTL::PipelineOptionBufferTypeInfo;
        state = CreateMetalRef<MTL::RenderPipelineState>(MetalDriver::GetCurrent()->GetDevice()->GetDevice()->
        newRenderPipelineState(descriptor, options, &reflection, &error));
        if (!state) {
            VWOLF_CORE_ERROR("Metal shader failed to load: %s.", error->localizedDescription()->utf8String());
            throw std::runtime_error(error->localizedDescription()->utf8String());
        }

        reflection->release();
        descriptor->release();
    }

    void MetalRenderState::PrepareBlending(MTL::RenderPipelineColorAttachmentDescriptor* descriptor) {
        descriptor->setBlendingEnabled(settings.GetBlendSettings().GetRenderTargets()[0].GetEnabled());
        descriptor->setRgbBlendOperation(GetOperation(settings.GetBlendSettings().GetRenderTargets()[0].GetEquation()));
        descriptor->setAlphaBlendOperation(GetOperation(settings.GetBlendSettings().GetRenderTargets()[0].GetEquation()));
        descriptor->setSourceRGBBlendFactor(MTL::BlendFactor::BlendFactorSourceAlpha);
        descriptor->setDestinationRGBBlendFactor(MTL::BlendFactor::BlendFactorOneMinusSourceAlpha);
        descriptor->setSourceAlphaBlendFactor(GetFactor(settings.GetBlendSettings().GetRenderTargets()[0].GetSourceFunction()));
        descriptor->setDestinationAlphaBlendFactor(GetFactor(settings.GetBlendSettings().GetRenderTargets()[0].GetDestinationFunction()));
        descriptor->setWriteMask(MTL::ColorWriteMaskAll);
    }

    void MetalRenderState::PrepareDepthStencilState() {
        MTL::DepthStencilDescriptor *descriptor = MTL::DepthStencilDescriptor::alloc()->init();

        descriptor->setDepthWriteEnabled(settings.GetDepthSettings().GetDepthTest());

        switch(settings.GetDepthSettings().GetDepthFunction()) {
            case Compare::Never:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionNever);
                break;
            case Compare::Less:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionLess);
                break;
            case Compare::LEqual:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
                break;
            case Compare::Equal:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionEqual);
                break;
            case Compare::NotEqual:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionNotEqual);
                break;
            case Compare::GEqual:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionGreaterEqual);
                break;
            case Compare::Greater:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionGreater);
                break;
            case Compare::Always:
                descriptor->setDepthCompareFunction(MTL::CompareFunctionAlways);
                break;
        }
        depthStencilState = CreateMetalRef<MTL::DepthStencilState>(MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newDepthStencilState(descriptor));
    }

}
#endif
