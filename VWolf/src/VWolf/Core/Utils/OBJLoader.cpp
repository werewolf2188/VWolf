//
//  OBJLoader.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/13/23.
//

#include "vwpch.h"
#include "OBJLoader.h"

#include "OBJ_Loader.h"

namespace VWolf {
    
    Vector2 ConvertToCoreVector2(objl::Vector2 vector) {
        return Vector2( vector.X, vector.Y );
    }

    Vector3 ConvertToCoreVector3(objl::Vector3 vector) {
        return Vector3( vector.X, vector.Y, vector.Z );
    }

    Vertex ConvertToCoreVertex(objl::Vertex vertex) {
        return Vertex(ConvertToCoreVector3(vertex.Position),
            Vector4(0, 0, 0, 0),
            ConvertToCoreVector3(vertex.Normal),
            Vector3( 0, 0, 0 ),
            ConvertToCoreVector2(vertex.TextureCoordinate));
    }

    MeshData OBJLoader::Load(std::string path, std::string name) {
        objl::Loader loader;

        MeshData meshData;

        if (loader.LoadFile(path)) {
            
            objl::Mesh curMesh = loader.LoadedMeshes[0];
            meshData.SetName(name);
            meshData.vertices.resize(curMesh.Vertices.size());
            for (int j = 0; j < curMesh.Vertices.size(); j++)
            {
                meshData.vertices[j] = ConvertToCoreVertex(curMesh.Vertices[j]);
            }
            meshData.indices.resize(curMesh.Indices.size());
            
            for (int j = 0; j < curMesh.Indices.size(); j += 3)
            {
                meshData.indices[j] = curMesh.Indices[j + 2];
                meshData.indices[j + 1] = curMesh.Indices[j + 1];
                meshData.indices[j + 2] = curMesh.Indices[j];
            }
        }
        
        return meshData;
    }

//    void OBJLoader::Test() {
//
//        objl::Loader loader;
//        if (loader.LoadFile("assets/basic_shapes/cube.obj")) {
//            VWOLF_CORE_INFO("Good");
//            // Go through each loaded mesh and out its contents
//            for (int i = 0; i < loader.LoadedMeshes.size(); i++)
//            {
//                // Copy one of the loaded meshes to be our current mesh
//                objl::Mesh curMesh = loader.LoadedMeshes[i];
//
//                // Print Mesh Name
//                std::cout << "Mesh " << i << ": " << curMesh.MeshName << "\n";
//
//                // Print Vertices
//                std::cout << "Vertices:\n";
//
//                // Go through each vertex and print its number,
//                //  position, normal, and texture coordinate
//                for (int j = 0; j < curMesh.Vertices.size(); j++)
//                {
//                    std::cout << "V" << j << ": " <<
//                        "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
//                        "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
//                        "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
//                }
//
//                // Print Indices
//                std::cout << "Indices:\n";
//
//                // Go through every 3rd index and print the
//                //    triangle that these indices represent
//                for (int j = 0; j < curMesh.Indices.size(); j += 3)
//                {
//                    std::cout << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
//                }
//
//                // Print Material
//                std::cout << "Material: " << curMesh.MeshMaterial.name << "\n";
//                std::cout << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
//                std::cout << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
//                std::cout << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
//                std::cout << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
//                std::cout << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
//                std::cout << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
//                std::cout << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
//                std::cout << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
//                std::cout << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
//                std::cout << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
//                std::cout << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
//                std::cout << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";
//
//                // Leave a space to separate from the next mesh
//                std::cout << "\n";
//            }
//        }
//    }
}
