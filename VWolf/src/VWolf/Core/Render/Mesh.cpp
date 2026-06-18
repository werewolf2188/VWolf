//
//  Mesh.cpp
//  VWolf
//
//  Created by Enrique Moises on 6/17/26.
//

#include "vwpch.h"
#include "Mesh.h"

#include "OBJ_Loader.h"

namespace VWolf {

    namespace {
        Vector2 Transform(objl::Vector2 vector) {
            return Vector2(vector.X, vector.Y);
        }
    
        Vector3 Transform(objl::Vector3 vector) {
            return Vector3(vector.X, vector.Y, vector.Z);
        }
    
        Vector3 GetVertex(objl::Vertex vertex) {
            return Transform(vertex.Position);
        }
    
        Vector3 GetNormal(objl::Vertex vertex) {
            return Transform(vertex.Normal);
        }
    
        Vector2 GetUV(objl::Vertex vertex) {
            return Transform(vertex.TextureCoordinate);
        }
    
        Color GetColor(objl::Vertex vertex) {
            return Color(1, 1, 1, 1);
        }
    }

    Mesh::Mesh(): Object(UUID::NewUUID()) {
        name = "unnamed";
    }

    Mesh::Mesh(objl::Loader& loader, UUID id): Object(id) {
        name = loader.LoadedMeshes[0].MeshName;
        
        triangles = loader.LoadedIndices;
        vertices.resize(loader.LoadedVertices.size());
        colors.resize(loader.LoadedVertices.size());
        normals.resize(loader.LoadedVertices.size());
        tangents.resize(loader.LoadedVertices.size());
        bitangents.resize(loader.LoadedVertices.size());
        uvs.resize(loader.LoadedVertices.size());
        
        std::transform(loader.LoadedVertices.begin(), loader.LoadedVertices.end(), vertices.begin(), GetVertex);
        std::transform(loader.LoadedVertices.begin(), loader.LoadedVertices.end(), colors.begin(), GetColor);
        std::transform(loader.LoadedVertices.begin(), loader.LoadedVertices.end(), normals.begin(), GetNormal);
        std::transform(loader.LoadedVertices.begin(), loader.LoadedVertices.end(), uvs.begin(), GetUV);
        
        RecalculateTangents();
    }

    void Mesh::RecalculateNormals() {
        normals.clear();
        normals.resize(vertices.size());
        std::transform(vertices.begin(), vertices.end(), normals.begin(), [](Vector3 vertex) {
            return vertex.Normalized();
        });
    }

    void Mesh::RecalculateTangents() {
        for(uint32_t index = 0; index < normals.size(); index++) {
            Vector3 refVec(0.0f, 0.0f, 1.0f);
            if (std::abs(Vector3::Dot(normals[index], refVec)) > 0.99f) {
                refVec = Vector3(1.0f, 0.0f, 0.0f);
            }

            Vector3 temp = refVec - Vector3::Dot(refVec, normals[index]) * normals[index];
            temp.Normalize();
            tangents[index] = temp;
            bitangents[index] = Vector3::Cross(normals[index], temp);
        }
    }

    Ref<Mesh> Mesh::Load(std::filesystem::path path, UUID id) {
        objl::Loader loader;
        
        if (loader.LoadFile(path.string()) && loader.LoadedMeshes.size() > 0) {
            Ref<Mesh> refM = CreateRef<Mesh>(loader, id);
            ObjectResourceManager::AddObject(id, refM);
            return refM;
        }
        
        throw std::exception();
    }
}
