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
        AbstractMaterial& material;
        MatrixFloat4x4 transform;

        RenderItem(MeshData& data,
                   AbstractMaterial& material,
                   MatrixFloat4x4 transform):
        data(data),
        material(material),
        transform(transform) {
            
        }
    };
}
