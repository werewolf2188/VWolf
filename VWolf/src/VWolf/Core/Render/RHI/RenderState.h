//
//  RenderState.h
//  VWolf
//
//  Created by Enrique Moises on 8/30/26.
//

#pragma once

#include "Types.h"
#include "Object.h"

namespace VWolf::RHI {
    enum class RasterizerCullMode {
        Back,
        Front,
        FrontAndBack
    };

    enum class RasterizerFillMode {
        Solid,
        Wireframe
    };

    class RasterizerSettings {
    public:
        RasterizerSettings(bool cullEnabled = true,
                           RasterizerFillMode fillMode = RasterizerFillMode::Solid,
                           RasterizerCullMode cullMode = RasterizerCullMode::Back,
                           bool counterClockwise = false):
        cullEnabled(cullEnabled), fillMode(fillMode), cullMode(cullMode), counterClockwise(counterClockwise) {}
        
        static RasterizerSettings FromRasterization(const VWolf::Rasterization& rasterization);
        
        const bool& GetCullEnabled() const { return cullEnabled; }
        const RasterizerFillMode GetFillMode() const { return fillMode; }
        const RasterizerCullMode GetCullMode() const { return cullMode; }
        const bool& GetCounterClockwise() const { return counterClockwise; }
    private:
        bool cullEnabled = true;
        RasterizerFillMode fillMode = RasterizerFillMode::Solid;
        RasterizerCullMode cullMode = RasterizerCullMode::Back;
        bool counterClockwise = false;
    };

    enum class BlendEquation {
        Add,
        Substract,
        ReverseSubstract,
        Min,
        Max
    };

    enum class BlendFunction {
        Zero, One,
        SrcColor, InvSrcColor,
        DstColor, InvDstColor,
        SrcAlpha, InvSrcAlpha,
        DstAlpha, InvDstAlpha,
        Src1Color, InvSrc1Color,
        Src1Alpha, InvSrc1Alpha,
        SrcAlphaSat,
        CnstColor, InvCnstColor,
        CnstAlpha, InvCnstAlpha
    };

    class RenderTargetSettings {
    public:
        RenderTargetSettings(bool enabled = true,
                             BlendEquation equation = BlendEquation::Add,
                             BlendFunction sourceFunction = BlendFunction::SrcAlpha,
                             BlendFunction destinationFunction = BlendFunction::InvSrcAlpha):
        enabled(enabled), equation(equation), sourceFunction(sourceFunction), destinationFunction(destinationFunction) {}
        
        static RenderTargetSettings FromBlend(const VWolf::Blend& blend);
        
        const bool GetEnabled() const { return enabled; }
        const BlendEquation GetEquation() const { return equation; }
        const BlendFunction GetSourceFunction() const { return sourceFunction; }
        const BlendFunction GetDestinationFunction() const { return destinationFunction; }
    private:
        bool enabled = true;
        BlendEquation equation = BlendEquation::Add;
        BlendFunction sourceFunction = BlendFunction::SrcAlpha;
        BlendFunction destinationFunction = BlendFunction::InvSrcAlpha;
    };

    class BlendSettings {
    public:
        BlendSettings(const RenderTargetSettings& target) {
            renderTargets[0] = target;
        }
        BlendSettings(std::array<RenderTargetSettings, 8> render_targets = {}): isIndependent(true), renderTargets(renderTargets) {}
        
        const bool& IsIndependent() const { return isIndependent; };
        const std::array<RenderTargetSettings, 8>& GetRenderTargets() const { return renderTargets; }
        
    private:
        bool isIndependent = false;
        std::array<RenderTargetSettings, 8> renderTargets {};
    };

    class DepthSettings {
    public:
        DepthSettings(bool depthTest = true,
                      Compare depthFunction = Compare::Less):
        depthTest(depthTest), depthFunction(depthFunction) {}

        static DepthSettings FromDepthStencil(const VWolf::DepthStencil& ds);
        
        const bool& GetDepthTest() const { return depthTest; }
        const Compare& GetDepthFunction() const { return depthFunction; }
    private:
        bool depthTest = true;
        Compare depthFunction = Compare::Less;
    };

    class RenderContext;
    class Program;
    class RenderPattern;

    class RenderStateSettings {
    public:
        RenderStateSettings() = default;
        RenderStateSettings(Ref<Program> program,
                            Weak<RenderPattern> pattern,
                            RasterizerSettings rasterizerSettings,
                            BlendSettings blendSettings,
                            DepthSettings depthSettings):
        program(program), pattern(pattern), rasterizerSettings(rasterizerSettings), blendSettings(blendSettings),
        depthSettings(depthSettings) {}
        
        const Ref<Program> GetProgram() const { return program; }
        const Weak<RenderPattern> GetPattern() const { return pattern; }
        const RasterizerSettings& GetRasterizerSettings() const { return rasterizerSettings; }
        const BlendSettings& GetBlendSettings() const { return blendSettings; }
        const DepthSettings& GetDepthSettings() const { return depthSettings;}
    private:
        Ref<Program> program;
        Weak<RenderPattern> pattern;
        RasterizerSettings rasterizerSettings;
        BlendSettings blendSettings;
        DepthSettings depthSettings;
    };

    class RenderState: public Object {
    public:
        RenderState(Weak<RenderContext> context, const RenderStateSettings& settings):
        context(context), settings(settings) {}
        virtual ~RenderState() {}
        
        const RenderStateSettings& GetSettings() const { return settings; }
        const Weak<RenderContext> GetContext() const { return context; }
    protected:
        RenderStateSettings settings;
        Weak<RenderContext> context;
    private:
    };
}
