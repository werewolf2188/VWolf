//
//  MetalDriver.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalDriver.h"

#include "VWolf/Platform/Metal/Windows/CocoaWindow.h"

#include "VWolf/Core/UI/UIManager.h"
#include "VWolf/Platform/Metal/UI/MetalUIManager.h"

#include "VWolf/Core/Render/Graphics.h"
#include "VWolf/Platform/Metal/Render/MetalGraphics.h"

#include <mach/mach_time.h>

namespace VWolf {
    class CocoaTime : public Time {
    public:
        CocoaTime() {
            mach_timebase_info_data_t info;
            mach_timebase_info(&info);
            frequency = (info.denom * 1e9) / info.numer;
            offset = mach_absolute_time();
        }
    protected:
        virtual float GetTime() override {            
            return (mach_absolute_time() - offset) / frequency;
        }
    private:
        uint64_t frequency;
        uint64_t offset;
    };

    MetalDriver* MetalDriver::currentDriver = nullptr;

    void MetalDriver::Initialize(InitConfiguration config, WindowEventCallback& callback) {
        this->callback = &callback;
        // Using GLFW instead of normal application since its too complicated to do it the normal way
        glfwInit();
        glfwSetErrorCallback(GLFWErrorCallback);
        window = CreateRef<CocoaWindow>(config, callback);
        window->Initialize();
        currentDriver = this;

        device = MDevice::GetCurrentDevice();
        surface = CreateRef<MSurface>(device, config.width, config.height);
        command = CreateRef<MCommand>(device);

        auto view = ConnectLayer(reinterpret_cast<NS::Window*>(MetalDriver::GetCurrent()->GetWindow()->GetNativeWindow()),
                                 surface->GetLayer());
        dynamic_cast<VWolf::CocoaWindow*>(MetalDriver::GetCurrent()->GetWindow().get())->SetView(view);

        graphics = CreateRef<MetalGraphics>();
        UIManager::SetDefault(CreateRef<MetalUIManager>());
        Graphics::SetGraphicsImpl(graphics);
        Time::SetTimeImplementation(CreateRef<CocoaTime>());
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
        surface->GetLayer()->setDrawableSize({static_cast<CGFloat>(m_Width), static_cast<CGFloat>(m_Height)});
    }
}

#endif

