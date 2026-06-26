//
//  Texture.cpp
//  VWolf
//
//  Created by Enrique Moises on 6/26/26.
//

#include "Texture.h"

namespace VWolf {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(TextureWrapMode, None, Repeat, Clamp, Mirror, MirrorOnce);
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(TextureFilterMode, Point, Bilinear, Trilinear);
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER(TextureOptions);
}
