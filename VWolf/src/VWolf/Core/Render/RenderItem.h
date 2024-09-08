//
//  RenderItem.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"

#include "RenderStructs.h"

namespace VWolf {
    struct RenderItem {
        MeshData& data;
        Material& material;
        Matrix4x4 transform;
        Ref<Camera> camera;

        RenderItem(MeshData& data,
                   Material& material,
                   Matrix4x4 transform,
                   Ref<Camera> camera):
        data(data),
        material(material),
        transform(transform),
        camera(camera) {
            
        }
    };
}
