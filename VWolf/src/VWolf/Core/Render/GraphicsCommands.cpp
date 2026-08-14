//
//  GraphicsCommands.cpp
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#include "vwpch.h"
#include "GraphicsCommands.h"

namespace VWolf {
    MeshGraphicsCommand::MeshGraphicsCommand(Ref<Mesh> mesh, Vector3 position, Quaternion rotation): mesh(mesh), transform(Matrix4x4::TRS(position, rotation, Vector3::One)) {
        
    }

    MeshGraphicsCommand::MeshGraphicsCommand(Ref<Mesh> mesh, Matrix4x4 transform): mesh(mesh), transform(transform) {
        
    }

    DrawMeshCommand::DrawMeshCommand(Ref<Mesh> mesh, Vector3 position, Quaternion rotation, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> camera, bool castShadows, bool receiveShadows): MeshGraphicsCommand(mesh, position, rotation), material(material), layer(layer), submeshIndex(submeshIndex), camera(camera), castShadows(castShadows), receiveShadows(receiveShadows) {

    }

    DrawMeshCommand::DrawMeshCommand(Ref<Mesh> mesh, Matrix4x4 transform, Ref<Material> material, int layer, int submeshIndex, Ref<CameraComponent> camera, bool castShadows, bool receiveShadows): MeshGraphicsCommand(mesh, transform), material(material), layer(layer), submeshIndex(submeshIndex), camera(camera), castShadows(castShadows), receiveShadows(receiveShadows) {

    }
}
