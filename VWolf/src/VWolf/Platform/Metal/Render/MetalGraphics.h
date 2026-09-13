//
//  MetalGraphics.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#include "VWolf/Core/Render/InternalGraphics.h"

#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Platform/Metal/Render/MetalTexture.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalRenderCommandList.h"

#include "VWolf/Core/Render/GraphicsContext.h"

namespace VWolf {

    class MetalGraphics: public InternalGraphics {
    public:
        MetalGraphics() = default;
        virtual ~MetalGraphics() override {};
    public:
        virtual void Initialize() override;
    public:
        Ref<RHI::MetalRenderCommandList> GetCommandList() { return commandList; }
    protected:
        void ClearColorImpl(Color color);

        virtual void EndProcessingFrame() override;
        virtual void BeginProcessingFrame() override;
    protected:
        virtual void DrawShadowMap() override;
        virtual void DrawQueue() override;
        virtual void DrawPostProcess() override;
    private:
        NS::AutoreleasePool* pool;
        Ref<MTL::CaptureScope> captureScope;
        
        Ref<RHI::MetalRenderCommandList> commandList;
        
        std::map<UUID, Matrix4x4> objectTransforms;
        Ref<MetalTexture2D> emptyShadowMap;
        Ref<MetalRenderTexture> shadowMap;
        
        Ref<CommandsContext> commandContext;
    };
}
