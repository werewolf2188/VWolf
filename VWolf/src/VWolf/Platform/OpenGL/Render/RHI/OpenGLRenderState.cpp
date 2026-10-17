//
//  OpenGLRenderState.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/12/26.
//

#include "vwpch.h"
#include "OpenGLRenderState.h"
#include "OpenGLProgram.h"

namespace VWolf::RHI {
    OpenGLRenderState::OpenGLRenderState(Weak<RenderContext> context, const RenderStateSettings& settings):
    RenderState(context, settings) {
        
    }

    OpenGLRenderState::~OpenGLRenderState() {
        
    }

    void OpenGLRenderState::Bind() {
        DynamicAs<OpenGLProgram>(GetSettings().GetProgram())->Bind();
        SetConfiguration();
    }

    void OpenGLRenderState::Unbind() {
        DynamicAs<OpenGLProgram>(GetSettings().GetProgram())->Unbind();
    }

    void OpenGLRenderState::SetConfiguration() const {
        SetRasterization();
        SetBlend();
        SetDepthStencil();
    }

    GLuint OpenGLRenderState::GetBlendFunction(RHI::BlendFunction function) const {
        switch (function) {
            case RHI::BlendFunction::Zero: return GL_ZERO;
            case RHI::BlendFunction::One: return GL_ONE;
            case RHI::BlendFunction::SrcColor: return GL_SRC_COLOR;
            case RHI::BlendFunction::InvSrcColor: return GL_ONE_MINUS_SRC_COLOR;
            case RHI::BlendFunction::DstColor: return GL_DST_COLOR;
            case RHI::BlendFunction::InvDstColor: return GL_ONE_MINUS_DST_COLOR;
            case RHI::BlendFunction::SrcAlpha: return GL_SRC_ALPHA;
            case RHI::BlendFunction::InvSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case RHI::BlendFunction::DstAlpha: return GL_DST_ALPHA;
            case RHI::BlendFunction::InvDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
            case RHI::BlendFunction::Src1Color: return GL_SRC1_COLOR;
            case RHI::BlendFunction::InvSrc1Color: return GL_ONE_MINUS_SRC1_COLOR;
            case RHI::BlendFunction::Src1Alpha: return GL_SRC1_ALPHA;
            case RHI::BlendFunction::InvSrc1Alpha: return GL_ONE_MINUS_SRC1_ALPHA;
            case RHI::BlendFunction::SrcAlphaSat: return GL_SRC_ALPHA_SATURATE;
            case RHI::BlendFunction::CnstColor: return GL_CONSTANT_COLOR;
            case RHI::BlendFunction::InvCnstColor: return GL_ONE_MINUS_CONSTANT_COLOR;
            case RHI::BlendFunction::CnstAlpha: return GL_CONSTANT_ALPHA;
            case RHI::BlendFunction::InvCnstAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
        }
        return GL_ZERO;
    }

    void OpenGLRenderState::SetRasterization() const {
        // Rasterization
        bool cullEnabled = GetSettings().GetRasterizerSettings().GetCullEnabled();
        RasterizerCullMode cullMode = GetSettings().GetRasterizerSettings().GetCullMode();
        RasterizerFillMode fillMode = GetSettings().GetRasterizerSettings().GetFillMode();
        bool counterClockwise = GetSettings().GetRasterizerSettings().GetCounterClockwise();
        
        if (cullEnabled) {
            GLThrowIfFailed(glEnable(GL_CULL_FACE));
        }
        else {
            GLThrowIfFailed(glDisable(GL_CULL_FACE));
        }

        switch (cullMode) {
            case RasterizerCullMode::Front:
                GLThrowIfFailed(glCullFace(GL_FRONT));
                break;
            case RasterizerCullMode::Back:
                GLThrowIfFailed(glCullFace(GL_BACK));
                break;
            case RasterizerCullMode::FrontAndBack:
                GLThrowIfFailed(glCullFace(GL_FRONT_AND_BACK));
                break;
        }

        switch (fillMode) {
            case RasterizerFillMode::Wireframe:
                GLThrowIfFailed(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                break;
            case RasterizerFillMode::Solid:
                GLThrowIfFailed(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                break;
        }

        GLThrowIfFailed(glFrontFace(counterClockwise ? GL_CCW: GL_CW));
    }

    void OpenGLRenderState::SetBlend() const {
        if (GetSettings().GetBlendSettings().GetRenderTargets().size() == 0) return;
        
        RenderTargetSettings targetSettings = GetSettings().GetBlendSettings().GetRenderTargets()[0];
        
        // Blend
        bool enabled = targetSettings.GetEnabled();
        BlendEquation equation = targetSettings.GetEquation();
        BlendFunction sourceFunc = targetSettings.GetSourceFunction();
        BlendFunction destinationFunc = targetSettings.GetDestinationFunction();
        
        if (enabled) {
            GLThrowIfFailed(glEnable(GL_BLEND));
        }
        else {
            GLThrowIfFailed(glDisable(GL_BLEND));
        }

        GLuint sourceFunction, destinationFunction;
        sourceFunction = GetBlendFunction(sourceFunc);
        destinationFunction = GetBlendFunction(destinationFunc);
        GLThrowIfFailed(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, sourceFunction, destinationFunction));
        
        switch (equation) {
            case BlendEquation::Add:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_ADD));
                break;
            case BlendEquation::Substract:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_SUBTRACT));
                break;
            case BlendEquation::ReverseSubstract:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_REVERSE_SUBTRACT));
                break;
            case BlendEquation::Min:
                GLThrowIfFailed(glBlendEquation(GL_MIN));
                break;
            case BlendEquation::Max:
                GLThrowIfFailed(glBlendEquation(GL_MAX));
                break;
        }
    }

    void OpenGLRenderState::SetDepthStencil() const {
        // Depth/Stencil
        bool depthTest = GetSettings().GetDepthSettings().GetDepthTest();
        Compare depthFunction = GetSettings().GetDepthSettings().GetDepthFunction();
        
        if (depthTest) {
            GLThrowIfFailed(glEnable(GL_DEPTH_TEST));
        }
        else {
            GLThrowIfFailed(glDisable(GL_DEPTH_TEST));
        }

        switch(depthFunction) {
            case Compare::Never:
                GLThrowIfFailed(glDepthFunc(GL_NEVER));
                break;
            case Compare::Less:
                GLThrowIfFailed(glDepthFunc(GL_LESS));
                break;
            case Compare::LEqual:
                GLThrowIfFailed(glDepthFunc(GL_LEQUAL));
                break;
            case Compare::Equal:
                GLThrowIfFailed(glDepthFunc(GL_EQUAL));
                break;
            case Compare::NotEqual:
                GLThrowIfFailed(glDepthFunc(GL_NOTEQUAL));
                break;
            case Compare::GEqual:
                GLThrowIfFailed(glDepthFunc(GL_GEQUAL));
                break;
            case Compare::Greater:
                GLThrowIfFailed(glDepthFunc(GL_GREATER));
                break;
            case Compare::Always:
                GLThrowIfFailed(glDepthFunc(GL_ALWAYS));
                break;
        }
    }
}
