//
//  MetalDriver.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalDriver.h"

#include "VWolf/Core/UI/UIManager.h"
#include "VWolf/Platform/Metal/UI/MetalUIManager.h"

#include "VWolf/Core/Render/Graphics.h"
#include "VWolf/Platform/Metal/Render/MetalGraphics.h"

#include <mach/mach_time.h>

namespace VWolf {
    Ref<MetalDriver> MetalDriver::currentDriver = nullptr;

    extern void SetView(Ref<Window> window, NS::View* view);

    static RHI::RenderPatternSettings TransformFromContextSettings(RHI::RenderContextSettings settings) {
        return RHI::RenderPatternSettings({
                                              RHI::RenderPassColorAttachment(settings.GetColorFormat(),
                                                                             RHI::RenderPassAttachment::LoadAction::Clear)
                                          },
                                          RHI::RenderPassDepthAttachment(settings.GetDepthStencilFormat(),
                                                                         RHI::RenderPassAttachment::LoadAction::Clear,
                                                                         RHI::RenderPassAttachment::StoreAction::DontCare),
                                          std::optional<RHI::RenderPassStencilAttachment>()
                                          );
    }

    void MetalDriver::Initialize(InitConfiguration config) {
        window = CreateGenericWindow(DriverType::Metal, config);
        currentDriver = UnownedRef<MetalDriver>(this);

        registry = CreateRef<RHI::ObjectRegistry>();
        RHI::RenderContextSettings renderContextSettings = RHI::RenderContextSettings(config.width, config.height);
        
        device = RHI::MetalDevice::GetCurrentDevice();
        renderContext = DynamicAs<RHI::MetalRenderContext>(device->CreateRenderContext(renderContextSettings));
        commandQueue = DynamicAs<RHI::MetalCommandQueue>(renderContext->CreateCommandQueue());
        
        RHI::TextureSettings surfaceColorTextureSettings = RHI::TextureSettings::ForFrameBuffer(renderContextSettings);
        RHI::TextureSettings surfaceDepthTextureSettings = RHI::TextureSettings::ForDepthStencil(renderContextSettings);
        
        surfaceColorTexture = DynamicAs<RHI::MetalTexture>(renderContext->CreateTexture(surfaceColorTextureSettings));
        surfaceDepthTexture = DynamicAs<RHI::MetalTexture>(renderContext->CreateTexture(surfaceDepthTextureSettings));
        
        auto view = ConnectLayer(reinterpret_cast<NS::Window*>(MetalDriver::GetCurrent()->GetWindow()->GetNativeWindow()),
                                 surfaceColorTexture->GetLayer().get());
        SetView(MetalDriver::GetCurrent()->GetWindow(), view);
        
        RHI::RenderPatternSettings surfaceRenderPatternSettings = TransformFromContextSettings(renderContextSettings);
        RHI::RenderPassSettings passSettings = RHI::RenderPassSettings({ surfaceColorTexture->GetTextureView({}), surfaceDepthTexture->GetTextureView({}) },
                                                                       config.width, config.height);
        
        mainRenderPattern = DynamicAs<RHI::MetalRenderPattern>(renderContext->CreateRenderPattern(surfaceRenderPatternSettings));
        mainRenderPass = DynamicAs<RHI::MetalRenderPass>(mainRenderPattern->CreateRenderPass(passSettings));

        graphics = CreateRef<MetalGraphics>();
        UIManager::SetDefault(CreateRef<MetalUIManager>());
        InternalGraphics::SetSingleton(graphics);
        graphics->Initialize();
    }
    void MetalDriver::Shutdown() {   
    }
    void MetalDriver::OnUpdate() {
        window->OnUpdate();
    }
    void MetalDriver::Resize(unsigned int m_Width, unsigned int m_Height) {
        RHI::RenderContextSettings renderContextSettings = RHI::RenderContextSettings(m_Width, m_Height);
        renderContext->SetSettings(renderContextSettings);
        RHI::TextureSettings surfaceColorTextureSettings = RHI::TextureSettings::ForFrameBuffer(renderContextSettings);
        RHI::TextureSettings surfaceDepthTextureSettings = RHI::TextureSettings::ForDepthStencil(renderContextSettings);
        
        surfaceColorTexture->Update(surfaceColorTextureSettings);
        surfaceDepthTexture->Update(surfaceDepthTextureSettings);
        
        mainRenderPass->Resize(m_Width, m_Height);
    }
}

#endif

