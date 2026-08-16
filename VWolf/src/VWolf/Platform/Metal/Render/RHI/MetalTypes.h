//
//  MetalTypes.h
//  VWolf
//
//  Created by Enrique Moises on 8/27/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/Types.h"
#include "VWolf/Core/Render/RHI/Resource.h"

namespace VWolf::RHI {
    const MTL::PixelFormat GetFormatFrom(const PixelFormat format);
    const MTL::TextureUsage GetTextureUsageFrom(const ResourceUsage usage);
}
