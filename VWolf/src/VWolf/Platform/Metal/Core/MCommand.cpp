//
//  MCommand.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/5/24.
//
#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MCommand.h"

namespace VWolf {
    MCommand::MCommand(Ref<MDevice> device) {
        commandQueue = device->GetDevice()->newCommandQueue();
    }

    MCommand::~MCommand() {
        commandQueue->release();
    }
}

#endif
