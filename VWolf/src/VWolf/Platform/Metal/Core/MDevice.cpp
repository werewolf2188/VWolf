//
//  MDevice.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//
#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MDevice.h"

namespace VWolf {
    Ref<MDevice> MDevice::GetCurrentDevice() {
//        NS::Array* devices = MTL::CopyAllDevices();
//        for(int i = 0; i < devices->count(); i++) {
//            MTL::Device* de = reinterpret_cast<MTL::Device*>(devices->object(i));
//            std::cout << "Test" << std::endl;
//        }
        return CreateRef<MDevice>(MTL::CreateSystemDefaultDevice());
    }

    MDevice::MDevice(MTL::Device* device): currentDevice(device) {}

    MDevice::~MDevice() {
        currentDevice->release();
    }
}

#endif
