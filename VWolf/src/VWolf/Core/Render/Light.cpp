//
//  Light.cpp
//  VWolf
//
//  Created by Enrique Moises on 5/16/26.
//

#include "vwpch.h"
#include "RenderStructs.h"

namespace VWolf {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(Light::LightType, Unknown, Directional, Spot, Point)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(Light)
}
