//
//  MDevice.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "Core.h"

namespace VWolf {
    class MDevice {
    public:
        static Ref<MDevice> GetCurrentDevice();
    public:
        MDevice(MTL::Device* device);
        ~MDevice();
    public:
        inline MTL::Device* GetDevice() { return currentDevice; }
    private:
        MTL::Device* currentDevice;
    };
}
