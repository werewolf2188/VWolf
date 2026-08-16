//
//  MetalDriver.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#include "VWolf/Core/Render/RHI/Driver.h"

#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalDevice.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderContext.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderPass.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalCommandQueue.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalTexture.h"

namespace VWolf {
    class MetalGraphics;

    class MetalDriver : public Driver {
    public:
        virtual ~MetalDriver() override {};
        virtual void Initialize(InitConfiguration config) override;
        virtual void Shutdown() override;
        virtual void OnUpdate() override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
    public:
        Ref<RHI::MetalDevice> GetDevice() { return device; }
        Ref<RHI::MetalRenderContext> GetRenderContext() { return renderContext; }
        Ref<RHI::MetalTexture> GetSurfaceColorTexture() { return surfaceColorTexture; }
        Ref<RHI::MetalRenderPattern> GetMainRenderPattern() { return mainRenderPattern; }
        Ref<RHI::MetalRenderPass> GetMainRenderPass() { return mainRenderPass; }
        Ref<RHI::MetalCommandQueue> GetCommandQueue() { return commandQueue; }
        Ref<RHI::ObjectRegistry> GetObjectRegistry() { return registry; }
        Ref<MetalGraphics> GetNativeGraphics() { return graphics; };
    public:
        static Ref<MetalDriver> GetCurrent() { return currentDriver; }
    private:
        static Ref<MetalDriver> currentDriver;
    private:
        Ref<RHI::ObjectRegistry> registry;
        Ref<RHI::MetalTexture> surfaceColorTexture, surfaceDepthTexture;
        Ref<RHI::MetalRenderPattern> renderPattern;
        Ref<RHI::MetalRenderPattern> mainRenderPattern;
        Ref<RHI::MetalRenderPass> mainRenderPass;
        
        Ref<RHI::MetalDevice> device;
        Ref<RHI::MetalRenderContext> renderContext;
        Ref<RHI::MetalCommandQueue> commandQueue;
        Ref<MetalGraphics> graphics;
    };
}
