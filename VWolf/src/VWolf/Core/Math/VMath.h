#pragma once

#include "Vector2.h"
#include "IVector2.h"
#include "Vector3.h"
#include "IVector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

#include "VWolf/Core/Color.h"

namespace VWolf {
    // TODO: Move to math class
    template<typename genType>
    inline constexpr genType radians(genType degrees)
    {
        static_assert(std::numeric_limits<genType>::is_iec559, "'radians' only accept floating-point input");

        return degrees * static_cast<genType>(0.01745329251994329576923690768489);
    }
}
