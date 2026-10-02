//
//  OpenGLTypes.h
//  VWolf
//
//  Created by Enrique Moises on 10/5/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"

#include "VWolf/Core/Render/RHI/Types.h"
#include "VWolf/Core/Render/RHI/Resource.h"

namespace VWolf::RHI {
    const uint32_t GetInternalFormatFrom(const PixelFormat format);
    const uint32_t GetDataFormatFrom(const PixelFormat format);
    const uint32_t GetDataTypeFrom(const PixelFormat format);
}
