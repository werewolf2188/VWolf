//
//  IIdentifiable.h
//  VWolf
//
//  Created by Enrique Moises on 4/23/26.
//

#pragma once

#include "VWolf/Core/Utils/UUID.h"

namespace VWolf {
    class IIdentifiable {
    public:
        const VWolf::UUID GetID() const { return id; }
        VWolf::UUID& GetID() { return id; }
        void SetID(VWolf::UUID value) { id = value; }
    protected:
        VWolf::UUID id;
    };
}
