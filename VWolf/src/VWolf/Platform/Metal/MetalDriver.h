//
//  MetalDriver.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#include "VWolf/Core/Driver.h"

#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Platform/Metal/Core/MDevice.h"
#include "VWolf/Platform/Metal/Core/MSurface.h"
#include "VWolf/Platform/Metal/Core/MCommand.h"

namespace VWolf {
    class MetalGraphics;

    class MetalDriver : public Driver, public WindowEventCallback {
    public:
        virtual ~MetalDriver() override {};
        virtual void Initialize(InitConfiguration config, WindowEventCallback& callback) override;
        virtual void Shutdown() override;
        virtual void OnUpdate() override;
        virtual void OnEvent(Event& evt) override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
    public:
        Ref<MDevice> GetDevice() { return device; }
        Ref<MSurface> GetSurface() { return surface; }
        Ref<MCommand> GetCommand() { return command; }
        Ref<MetalGraphics> GetNativeGraphics() { return graphics; };
    public:
        static MetalDriver* GetCurrent() { return currentDriver; }
    private:
        static MetalDriver* currentDriver;
    private:
        Ref<MDevice> device;
        Ref<MSurface> surface;
        Ref<MCommand> command;
        WindowEventCallback* callback;
        Ref<MetalGraphics> graphics;
        NS::AutoreleasePool* autoreleasePool;
    };
}
