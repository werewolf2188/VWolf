//
//  Types.h
//  VWolf
//
//  Created by Enrique Moises on 8/17/26.
//

#pragma once

namespace VWolf::RHI  {
    enum class DataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    enum class PixelFormat : uint32_t
    {
        Unknown = 0U,
        RGBA8,
        RGBA8Unorm,
        RGBA8Unorm_sRGB,
        BGRA8Unorm,
        BGRA8Unorm_sRGB,
        RGBA32Float,
        R32Float,
        R32Uint,
        R32Sint,
        R16Float,
        R16Uint,
        R16Sint,
        R16Unorm,
        R16Snorm,
        R8Uint,
        R8Sint,
        R8Unorm,
        R8Snorm,
        A8Unorm,
        Depth32Float,
        Depth32Float_Stencil8
    };

    enum class Compare {
        Never,
        Less,
        Equal,
        LEqual,
        Greater,
        NotEqual,
        GEqual,
        Always
    };
}
