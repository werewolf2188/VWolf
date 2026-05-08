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
    MetalDriver* MetalDriver::currentDriver = nullptr;

    extern void SetView(Ref<Window> window, NS::View* view);

    void MetalDriver::Initialize(InitConfiguration config, WindowEventCallback& callback) {
        this->callback = &callback;
        window = CreateGenericWindow(DriverType::Metal, config, callback);
        window->Initialize();
        currentDriver = this;

        device = MDevice::GetCurrentDevice();
        surface = CreateRef<MSurface>(device, config.width, config.height);
        command = CreateRef<MCommand>(device);

        auto view = ConnectLayer(reinterpret_cast<NS::Window*>(MetalDriver::GetCurrent()->GetWindow()->GetNativeWindow()),
                                 surface->GetLayer());
        SetView(MetalDriver::GetCurrent()->GetWindow(), view);

        graphics = CreateRef<MetalGraphics>();
        UIManager::SetDefault(CreateRef<MetalUIManager>());
        Graphics::SetGraphicsImpl(graphics);
        Time::SetTimeImplementation(CreateGenericTime());
        graphics->Initialize();
    }
    void MetalDriver::Shutdown() {
        autoreleasePool->release();        
    }
    void MetalDriver::OnUpdate() {
        window->OnUpdate();
    }
    void MetalDriver::OnEvent(Event& evt) {
        callback->OnEvent(evt);
    }
    void MetalDriver::Resize(unsigned int m_Width, unsigned int m_Height) {
        surface->Resize(device, m_Width, m_Height);
    }
}

#endif

