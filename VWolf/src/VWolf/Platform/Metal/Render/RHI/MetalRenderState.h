//
//  MetalRenderState.h
//  VWolf
//
//  Created by Enrique Moises on 9/2/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Render/RHI/RenderState.h"

namespace VWolf::RHI {
    class MetalRenderState: public RenderState {
    public:
        MetalRenderState(Weak<RenderContext> context, const RenderStateSettings& settings);
        ~MetalRenderState();
        
        Ref<MTL::RenderPipelineState> GetState() { return state; }
        Ref<MTL::DepthStencilState> GetDepthStencilState() { return depthStencilState; }
    private:
        void PrepareDepthStencilState();
        void ReflectLibraryAndCreateState();
        void PrepareBlending(MTL::RenderPipelineColorAttachmentDescriptor* descriptor);
        
        Ref<MTL::RenderPipelineState> state;
        Ref<MTL::DepthStencilState> depthStencilState;
    };
}
