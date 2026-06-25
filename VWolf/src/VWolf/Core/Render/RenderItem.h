//
//  RenderItem.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"

#include "Mesh.h"

namespace VWolf {
    struct RenderItem {
        Ref<Mesh> mesh;
        Material& material;
        Matrix4x4 transform;
        Ref<Camera> camera;

        RenderItem(Ref<Mesh> mesh,
                   Material& material,
                   Matrix4x4 transform,
                   Ref<Camera> camera):
        mesh(mesh),
        material(material),
        transform(transform),
        camera(camera) {
            
        }
    };
}
