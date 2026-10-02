#include "vwpch.h"
#include "OpenGLDriver.h"

#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/OpenGL/UI/OpenGLUIManager.h"

#include "VWolf/Core/Render/Graphics.h"
#include "VWolf/Platform/OpenGL/Render/OpenGLGraphics.h"

#include "VWolf/Core/Time.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {
	Ref<OpenGLDriver> OpenGLDriver::currentDriver = nullptr;

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

	void OpenGLDriver::Initialize(InitConfiguration config)
	{
        window = CreateGenericWindow(DriverType::OpenGL, config);
        currentDriver = UnownedRef<OpenGLDriver>(this);
        
		UIManager::SetDefault(CreateRef<OpenGLUIManager>());
        Ref<OpenGLGraphics> graphics = CreateRef<OpenGLGraphics>();
        InternalGraphics::SetSingleton(graphics);
        
        RHI::RenderContextSettings renderContextSettings = RHI::RenderContextSettings(config.width, config.height);

        registry = CreateRef<RHI::ObjectRegistry>();
        
        device = RHI::OpenGLDevice::GetCurrentDevice();
        renderContext = DynamicAs<RHI::OpenGLRenderContext>(device->CreateRenderContext(renderContextSettings));
        commandQueue = DynamicAs<RHI::OpenGLCommandQueue>(renderContext->CreateCommandQueue());
        
        RHI::RenderPatternSettings surfaceRenderPatternSettings = TransformFromContextSettings(renderContextSettings);
        mainRenderPattern = DynamicAs<RHI::OpenGLRenderPattern>(renderContext->CreateRenderPattern(surfaceRenderPatternSettings));
        
#ifdef VWOLF_PLATFORM_MACOS
        GLThrowIfFailed(glViewport(0, 0, window->GetFramebufferWidth(), window->GetFramebufferHeight()));
        this->m_Width = window->GetFramebufferWidth();
        this->m_Height = window->GetFramebufferHeight();
#else
        Ref<Monitor> mode = window->GetMonitor();
        VWOLF_CORE_INFO("Resolution %dx%d ", mode->GetVideoModeWidth(), mode->GetVideoModeHeight());
        GLThrowIfFailed(glViewport(0, 0, mode->GetVideoModeWidth(), mode->GetVideoModeHeight()));
        this->m_Width = mode->GetVideoModeWidth();
        this->m_Height = mode->GetVideoModeHeight();
#endif
        graphics->Initialize();
	}

	void OpenGLDriver::Shutdown()
	{
	}

	void OpenGLDriver::OnUpdate() {
		window->OnUpdate();
        ResizeIfNeeded();
	}

    void OpenGLDriver::Resize(unsigned int m_Width, unsigned int m_Height) {
        this->m_Width = m_Width;
        this->m_Height = m_Height;
        this->m_resize = true;
    }

    void OpenGLDriver::ResizeIfNeeded() {
        if (m_resize) {
#ifdef VWOLF_PLATFORM_MACOS
        GLThrowIfFailed(glViewport(0, 0, window->GetFramebufferWidth(), window->GetFramebufferHeight()));
#else
        GLThrowIfFailed(glViewport(0, 0, m_Width, m_Height));
#endif
            m_resize = false;
        }
    }
}
