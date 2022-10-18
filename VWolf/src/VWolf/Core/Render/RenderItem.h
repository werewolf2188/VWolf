//
//  RenderItem.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/Math.h"

#include "RenderStructs.h"

namespace VWolf {
    struct RenderItem {
        MeshData& data;
        std::string shaderName;
        MatrixFloat4x4 transform;

        RenderItem(MeshData& data, std::string shaderName, MatrixFloat4x4 transform): data(data), shaderName(shaderName), transform(transform) {
            
        }
    };
}
