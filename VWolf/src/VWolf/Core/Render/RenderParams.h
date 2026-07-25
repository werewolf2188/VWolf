//
//  RenderParams.h
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#pragma once

#include "Camera.h"
#include "Material.h"
#include "VWolf/Core/Utils/UUID.h"

namespace VWolf {
    struct RenderParams {
    public:
        RenderParams(Ref<Material> material);
    private:
        Ref<Camera> camera;
        UUID entityId;
        Ref<Material> material;
    };
}
