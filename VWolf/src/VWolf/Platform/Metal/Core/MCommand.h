//
//  MCommand.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/5/24.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "Core.h"
#include "MDevice.h"

namespace VWolf {
    class MCommand {
    public:
        MCommand(Ref<MDevice> device);
        ~MCommand();
    public:
        MTL::CommandQueue* GetCommandQueue() { return commandQueue; }
    private:
        MTL::CommandQueue* commandQueue;
    };
}
