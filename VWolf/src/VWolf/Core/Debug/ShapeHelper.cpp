//
//  ShapeHelper.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/4/22.
//

#include "vwpch.h"

#include "ShapeHelper.h"

#define TOP 1
#define BOTTOM -1

namespace VWolf {
    namespace {
        template<typename T>
        T Midpoint(T t1, T t2) {
            return 0.5f * (t1 + t2);
        }
    
        template<typename T>
        T NormalizedMidpoint(T t1, T t2) {
            return (0.5f * (t1 + t2)).Normalized();
        }
    
        void BuildCylinderCap(float radius, int cap, float height, std::uint32_t sliceCount, std::uint32_t stackCount, Mesh& meshData) {
            if (cap != TOP && cap != BOTTOM) return;
            std::uint32_t baseIndex = (std::uint32_t)meshData.GetVertices().size();
            float y = (0.5f * cap) * height;

            // vertices of ring
            float dTheta = 2.0f * M_PI / sliceCount;
            for(std::uint32_t i = 0; i <= sliceCount; ++i)
            {
                float x = radius * cosf(i * dTheta);
                float z = radius * sinf(i * dTheta);

                // Scale down by the height to try and make top cap texture coord area
                // proportional to base.
                float u = x / height + 0.5f;
                float v = z / height + 0.5f;

                meshData.GetVertices().push_back(Vector3(x, y, z));
                meshData.GetColors().push_back(Color(x, y, z, 1.0f));
                meshData.GetNormals().push_back(Vector3(0.0f, 1.0f * (cap == BOTTOM ? -1: 1), 0.0f));
                meshData.GetTangents().push_back(Vector3(1.0f, 0.0f, 0.0f));
                meshData.GetUVs().push_back(Vector2(u, v));
            }

            // Cap center vertex.
            meshData.GetVertices().push_back(Vector3(0.0f, y, 0.0f));
            meshData.GetColors().push_back(Color(0.0f, 1.0f, 0.0f, 1.0f));
            meshData.GetNormals().push_back(Vector3(0.0f, 1.0f * (cap == BOTTOM ? -1: 1), 0.0f));
            meshData.GetTangents().push_back(Vector3(1.0f, 0.0f, 0.0f));
            meshData.GetUVs().push_back(Vector2(0.5f, 0.5f));

            // Cache the index of center vertex.
            std::uint32_t centerIndex = (std::uint32_t)meshData.GetVertices().size() - 1;

            for(std::uint32_t i = 0; i < sliceCount; ++i)
            {
                if (cap == TOP) {
                    meshData.GetTriangles().push_back(baseIndex + i);
                    meshData.GetTriangles().push_back(baseIndex + i + 1);
                    meshData.GetTriangles().push_back(centerIndex);
                } else if (cap == BOTTOM) {
                    meshData.GetTriangles().push_back(baseIndex + i + 1);
                    meshData.GetTriangles().push_back(baseIndex + i);
                    meshData.GetTriangles().push_back(centerIndex);
                }
                
            }
        }

        void Subdivide(Mesh& meshData) {
            Mesh inputCopy = meshData;

            meshData.GetVertices().resize(0);
            meshData.GetTriangles().resize(0);

            //       v1
            //       *
            //      / \
            //     /   \
            //  m0*-----*m1
            //   / \   / \
            //  /   \ /   \
            // *-----*-----*
            // v0    m2     v2

            uint32_t numTris = (uint32_t)inputCopy.GetTriangles().size()/3;
            for(uint32_t i = 0; i < numTris; ++i)
            {
                Vector3 position0 = inputCopy.GetVertices()[ inputCopy.GetTriangles()[i * 3 + 0] ];
                Vector3 position1 = inputCopy.GetVertices()[ inputCopy.GetTriangles()[i * 3 + 1] ];
                Vector3 position2 = inputCopy.GetVertices()[ inputCopy.GetTriangles()[i * 3 + 2] ];
                
                Color color0 = inputCopy.GetColors()[ inputCopy.GetTriangles()[i * 3 + 0] ];
                Color color1 = inputCopy.GetColors()[ inputCopy.GetTriangles()[i * 3 + 1] ];
                Color color2 = inputCopy.GetColors()[ inputCopy.GetTriangles()[i * 3 + 2] ];
                
                Vector3 normal0 = inputCopy.GetNormals()[ inputCopy.GetTriangles()[i * 3 + 0] ];
                Vector3 normal1 = inputCopy.GetNormals()[ inputCopy.GetTriangles()[i * 3 + 1] ];
                Vector3 normal2 = inputCopy.GetNormals()[ inputCopy.GetTriangles()[i * 3 + 2] ];
                
                Vector3 tangent0 = inputCopy.GetTangents()[ inputCopy.GetTriangles()[i * 3 + 0] ];
                Vector3 tangent1 = inputCopy.GetTangents()[ inputCopy.GetTriangles()[i * 3 + 1] ];
                Vector3 tangent2 = inputCopy.GetTangents()[ inputCopy.GetTriangles()[i * 3 + 2] ];
                
                Vector2 uv0 = inputCopy.GetUVs()[ inputCopy.GetTriangles()[i * 3 + 0] ];
                Vector2 uv1 = inputCopy.GetUVs()[ inputCopy.GetTriangles()[i * 3 + 1] ];
                Vector2 uv2 = inputCopy.GetUVs()[ inputCopy.GetTriangles()[i * 3 + 2] ];

                //
                // Generate the midpoints.
                //
                Vector3 p0 = Midpoint(position0, position1);
                Vector3 p1 = Midpoint(position1, position2);
                Vector3 p2 = Midpoint(position0, position2);
                
                Color c0 = Midpoint(color0, color1);
                Color c1 = Midpoint(color1, color2);
                Color c2 = Midpoint(color0, color2);
                
                Vector3 n0 = Midpoint(normal0, normal1);
                Vector3 n1 = Midpoint(normal1, normal2);
                Vector3 n2 = Midpoint(normal0, normal2);
                
                Vector3 t0 = Midpoint(tangent0, tangent1);
                Vector3 t1 = Midpoint(tangent1, tangent2);
                Vector3 t2 = Midpoint(tangent0, tangent2);
                
                Vector2 u0 = Midpoint(uv0, uv1);
                Vector2 u1 = Midpoint(uv1, uv2);
                Vector2 u2 = Midpoint(uv0, uv2);

                //
                // Add new geometry.
                //
                
                meshData.GetVertices().push_back(position0); // 0
                meshData.GetColors().push_back(color0); // 0
                meshData.GetNormals().push_back(normal0); // 0
                meshData.GetTangents().push_back(tangent0); // 0
                meshData.GetUVs().push_back(uv0); // 0
                
                meshData.GetVertices().push_back(position1); // 1
                meshData.GetColors().push_back(color1); // 1
                meshData.GetNormals().push_back(normal1); // 1
                meshData.GetTangents().push_back(tangent1); // 1
                meshData.GetUVs().push_back(uv1); // 1
                
                meshData.GetVertices().push_back(position2); // 2
                meshData.GetColors().push_back(color2); // 2
                meshData.GetNormals().push_back(normal2); // 2
                meshData.GetTangents().push_back(tangent2); // 2
                meshData.GetUVs().push_back(uv2); // 2
                
                meshData.GetVertices().push_back(p0); // 3
                meshData.GetColors().push_back(c0); // 3
                meshData.GetNormals().push_back(n0); // 3
                meshData.GetTangents().push_back(t0); // 3
                meshData.GetUVs().push_back(u0); // 3
                
                meshData.GetVertices().push_back(p1); // 4
                meshData.GetColors().push_back(c1); // 4
                meshData.GetNormals().push_back(n1); // 4
                meshData.GetTangents().push_back(t1); // 4
                meshData.GetUVs().push_back(u1); // 4
                
                meshData.GetVertices().push_back(p2); // 5
                meshData.GetColors().push_back(c2); // 5
                meshData.GetNormals().push_back(n2); // 5
                meshData.GetTangents().push_back(t2); // 5
                meshData.GetUVs().push_back(u2); // 5
         
                meshData.GetTriangles().push_back(i * 6 + 0);
                meshData.GetTriangles().push_back(i * 6 + 3);
                meshData.GetTriangles().push_back(i * 6 + 5);

                meshData.GetTriangles().push_back(i * 6 + 3);
                meshData.GetTriangles().push_back(i * 6 + 4);
                meshData.GetTriangles().push_back(i * 6 + 5);

                meshData.GetTriangles().push_back(i * 6 + 5);
                meshData.GetTriangles().push_back(i * 6 + 4);
                meshData.GetTriangles().push_back(i * 6 + 2);

                meshData.GetTriangles().push_back(i * 6 + 3);
                meshData.GetTriangles().push_back(i * 6 + 1);
                meshData.GetTriangles().push_back(i * 6 + 4);
            }
        }
    }

    std::tuple<std::string, UUID> ShapeHelper::Box = std::make_tuple<std::string, UUID>("Box", UUID::NewUUID());

    std::tuple<std::string, UUID> ShapeHelper::Sphere = std::make_tuple<std::string, UUID>("Sphere", UUID::NewUUID());

    std::tuple<std::string, UUID> ShapeHelper::Geosphere = std::make_tuple<std::string, UUID>("Geosphere", UUID::NewUUID());

    std::tuple<std::string, UUID> ShapeHelper::Cylinder = std::make_tuple<std::string, UUID>("Cylinder", UUID::NewUUID());

    std::tuple<std::string, UUID> ShapeHelper::Grid = std::make_tuple<std::string, UUID>("Grid", UUID::NewUUID());

    std::tuple<std::string, UUID> ShapeHelper::Quad = std::make_tuple<std::string, UUID>("Quad", UUID::NewUUID());

    std::tuple<std::string, UUID> ShapeHelper::Triangle = std::make_tuple<std::string, UUID>("Triangle", UUID::NewUUID());

    std::tuple<std::string, UUID> ShapeHelper::Monkey = std::make_tuple<std::string, UUID>("Monkey", UUID::NewUUID());

    MeshData ShapeHelper::Create(std::string name) {
        if (VWOLF_GET_SHAPE_NAME(ShapeHelper::Box) == name) {
            return ShapeHelper::CreateBox(1, 1, 1, 0);
        }
        else if (VWOLF_GET_SHAPE_NAME(ShapeHelper::Sphere) == name) {
            return ShapeHelper::CreateSphere(2, 32, 32);
        }
        else if (VWOLF_GET_SHAPE_NAME(ShapeHelper::Geosphere) == name) {
            return ShapeHelper::CreateGeosphere(1, 4);
        }
        else if (VWOLF_GET_SHAPE_NAME(ShapeHelper::Cylinder) == name) {
            return ShapeHelper::CreateCylinder(1, 1, 3, 32, 8);
        }
        else if (VWOLF_GET_SHAPE_NAME(ShapeHelper::Grid) == name) {
            return ShapeHelper::CreateGrid(2, 2, 16, 16);
        }
        else if (VWOLF_GET_SHAPE_NAME(ShapeHelper::Quad) == name) {
            return ShapeHelper::CreateQuad(50, 50, 100, 100, 10);
        }
        else if (VWOLF_GET_SHAPE_NAME(ShapeHelper::Triangle) == name) {
            return ShapeHelper::CreateTriangle();
        }
        return MeshData();
    }

    Mesh CreateBox(float width, float height, float depth, std::uint32_t numSubdivisions) {
        Mesh meshData;
        meshData.SetName("Box");
        
        std::vector<Vector3> vertices(24);
        std::vector<Color> colors(24);
        std::vector<Vector3> normals(24);
        std::vector<Vector3> tangents(24);
        std::vector<Vector2> uvs(24);
        
        Vertex v[24];

        float w2 = 0.5f*width;
        float h2 = 0.5f*height;
        float d2 = 0.5f*depth;

        // Fill in the front face vertex data.
        vertices[0] = Vector3(-w2, -h2, -d2); colors[0] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[0] = Vector3(0.0f, 0.0f, -1.0f); tangents[0] = Vector3(1.0f, 0.0f, 0.0f); uvs[0] = Vector2(0.0f, 1.0f);
        vertices[1] = Vector3(-w2, +h2, -d2); colors[1] = Color(0.0f, 1.0f, 0.0f, 1.0f); normals[1] = Vector3(0.0f, 0.0f, -1.0f); tangents[1] = Vector3(1.0f, 0.0f, 0.0f); uvs[1] = Vector2(0.0f, 0.0f);
        vertices[2] = Vector3(+w2, +h2, -d2); colors[2] = Color(0.0f, 0.0f, 1.0f, 1.0f); normals[2] = Vector3(0.0f, 0.0f, -1.0f); tangents[2] = Vector3(1.0f, 0.0f, 0.0f); uvs[2] = Vector2(1.0f, 0.0f);
        vertices[3] = Vector3(+w2, -h2, -d2); colors[3] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[3] = Vector3(0.0f, 0.0f, -1.0f); tangents[3] = Vector3(1.0f, 0.0f, 0.0f); uvs[3] = Vector2(1.0f, 1.0f);
        
        // Fill in the back face vertex data.
        vertices[4] = Vector3(-w2, -h2, +d2); colors[4] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[4] = Vector3(0.0f, 0.0f, 1.0f); tangents[4] = Vector3(-1.0f, 0.0f, 0.0f); uvs[4] = Vector2(1.0f, 1.0f);
        vertices[5] = Vector3(+w2, -h2, +d2); colors[5] = Color(0.0f, 1.0f, 0.0f, 1.0f); normals[5] = Vector3(0.0f, 0.0f, 1.0f); tangents[5] = Vector3(-1.0f, 0.0f, 0.0f); uvs[5] = Vector2(0.0f, 1.0f);
        vertices[6] = Vector3(+w2, +h2, +d2); colors[6] = Color(0.0f, 0.0f, 1.0f, 1.0f); normals[6] = Vector3(0.0f, 0.0f, 1.0f); tangents[6] = Vector3(-1.0f, 0.0f, 0.0f); uvs[6] = Vector2(0.0f, 0.0f);
        vertices[7] = Vector3(-w2, +h2, +d2); colors[7] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[7] = Vector3(0.0f, 0.0f, 1.0f); tangents[7] = Vector3(-1.0f, 0.0f, 0.0f); uvs[7] = Vector2(1.0f, 0.0f);

        // Fill in the top face vertex data.
        vertices[8]  = Vector3(-w2, -h2, -d2); colors[8]  = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[8]  = Vector3(0.0f, 1.0f, 0.0f); tangents[8]  = Vector3(1.0f, 0.0f, 0.0f); uvs[8]  = Vector2(0.0f, 1.0f);
        vertices[9]  = Vector3(+w2, -h2, -d2); colors[9]  = Color(0.0f, 1.0f, 0.0f, 1.0f); normals[9]  = Vector3(0.0f, 1.0f, 0.0f); tangents[9]  = Vector3(1.0f, 0.0f, 0.0f); uvs[9]  = Vector2(0.0f, 0.0f);
        vertices[10] = Vector3(+w2, -h2, +d2); colors[10] = Color(0.0f, 0.0f, 1.0f, 1.0f); normals[10] = Vector3(0.0f, 1.0f, 0.0f); tangents[10] = Vector3(1.0f, 0.0f, 0.0f); uvs[10] = Vector2(1.0f, 0.0f);
        vertices[11] = Vector3(-w2, -h2, +d2); colors[11] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[11] = Vector3(0.0f, 1.0f, 0.0f); tangents[11] = Vector3(1.0f, 0.0f, 0.0f); uvs[11] = Vector2(1.0f, 1.0f);

        // Fill in the bottom face vertex data.
        vertices[12] = Vector3(-w2, -h2, -d2); colors[12] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[12] = Vector3(0.0f, -1.0f, 0.0f); tangents[12] = Vector3(-1.0f, 0.0f, 0.0f); uvs[12] = Vector2(1.0f, 1.0f);
        vertices[13] = Vector3(+w2, -h2, -d2); colors[13] = Color(0.0f, 1.0f, 0.0f, 1.0f); normals[13] = Vector3(0.0f, -1.0f, 0.0f); tangents[13] = Vector3(-1.0f, 0.0f, 0.0f); uvs[13] = Vector2(0.0f, 1.0f);
        vertices[14] = Vector3(+w2, -h2, +d2); colors[14] = Color(0.0f, 0.0f, 1.0f, 1.0f); normals[14] = Vector3(0.0f, -1.0f, 0.0f); tangents[14] = Vector3(-1.0f, 0.0f, 0.0f); uvs[14] = Vector2(0.0f, 0.0f);
        vertices[15] = Vector3(-w2, -h2, +d2); colors[15] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[15] = Vector3(0.0f, -1.0f, 0.0f); tangents[15] = Vector3(-1.0f, 0.0f, 0.0f); uvs[15] = Vector2(1.0f, 0.0f);

        // Fill in the left face vertex data.
        vertices[16] = Vector3(-w2, -h2, +d2); colors[16] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[16] = Vector3(-1.0f, 0.0f, 0.0f); tangents[16] = Vector3(0.0f, 0.0f, -1.0f); uvs[16] = Vector2(0.0f, 1.0f);
        vertices[17] = Vector3(-w2, +h2, +d2); colors[17] = Color(0.0f, 1.0f, 0.0f, 1.0f); normals[17] = Vector3(-1.0f, 0.0f, 0.0f); tangents[17] = Vector3(0.0f, 0.0f, -1.0f); uvs[17] = Vector2(0.0f, 0.0f);
        vertices[18] = Vector3(-w2, +h2, -d2); colors[18] = Color(0.0f, 0.0f, 1.0f, 1.0f); normals[18] = Vector3(-1.0f, 0.0f, 0.0f); tangents[18] = Vector3(0.0f, 0.0f, -1.0f); uvs[18] = Vector2(1.0f, 0.0f);
        vertices[19] = Vector3(-w2, -h2, -d2); colors[19] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[19] = Vector3(-1.0f, 0.0f, 0.0f); tangents[19] = Vector3(0.0f, 0.0f, -1.0f); uvs[19] = Vector2(1.0f, 1.0f);

        // Fill in the right face vertex data.
        vertices[20] = Vector3(+w2, -h2, -d2); colors[20] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[20] = Vector3(1.0f, 0.0f, 0.0f); tangents[20] = Vector3(0.0f, 0.0f, 1.0f); uvs[20] = Vector2(0.0f, 1.0f);
        vertices[21] = Vector3(+w2, +h2, -d2); colors[21] = Color(0.0f, 1.0f, 0.0f, 1.0f); normals[21] = Vector3(1.0f, 0.0f, 0.0f); tangents[21] = Vector3(0.0f, 0.0f, 1.0f); uvs[21] = Vector2(0.0f, 0.0f);
        vertices[22] = Vector3(+w2, +h2, +d2); colors[22] = Color(0.0f, 0.0f, 1.0f, 1.0f); normals[22] = Vector3(1.0f, 0.0f, 0.0f); tangents[22] = Vector3(0.0f, 0.0f, 1.0f); uvs[22] = Vector2(1.0f, 0.0f);
        vertices[23] = Vector3(+w2, -h2, +d2); colors[23] = Color(1.0f, 0.0f, 0.0f, 1.0f); normals[23] = Vector3(1.0f, 0.0f, 0.0f); tangents[23] = Vector3(0.0f, 0.0f, 1.0f); uvs[23] = Vector2(1.0f, 1.0f);

        std::vector<std::uint32_t> i(36);

        // Fill in the front face index data
        i[0] = 2; i[1] = 1; i[2] = 0;
        i[3] = 3; i[4] = 2; i[5] = 0;

        // Fill in the back face index data
        i[6] = 6; i[7]  = 5; i[8]  = 4;
        i[9] = 7; i[10] = 6; i[11] = 4;

        // Fill in the top face index data
        i[12] = 10; i[13] =  9; i[14] = 8;
        i[15] = 11; i[16] = 10; i[17] = 8;

        // Fill in the bottom face index data
        i[18] = 14; i[19] = 13; i[20] = 12;
        i[21] = 15; i[22] = 14; i[23] = 12;

        // Fill in the left face index data
        i[24] = 18; i[25] = 17; i[26] = 16;
        i[27] = 19; i[28] = 18; i[29] = 16;

        // Fill in the right face index data
        i[30] = 22; i[31] = 21; i[32] = 20;
        i[33] = 23; i[34] = 22; i[35] = 20;

        numSubdivisions = std::min<std::uint32_t>(numSubdivisions, 6u);
        for(std::uint32_t i = 0; i < numSubdivisions; ++i)
            Subdivide(meshData);

        meshData.SetVertices(vertices);
        meshData.SetColors(colors);
        meshData.SetNormals(normals);
        meshData.SetTangents(tangents);
        meshData.SetUVs(uvs);
        meshData.SetTriangles(i);
        
        return meshData;
    }

    Mesh CreateSphere(float radius, std::uint32_t sliceCount, std::uint32_t stackCount) {
        Mesh meshData;
        meshData.SetName("Sphere");
        //
        // Compute the vertices stating at the top pole and moving down the stacks.
        //
        
        std::vector<Vector3> vertices;
        std::vector<Color> colors;
        std::vector<Vector3> normals;
        std::vector<Vector3> tangents;
        std::vector<Vector2> uvs;

        // Poles: note that there will be texture coordinate distortion as there is
        // not a unique point on the texture map to assign to the pole when mapping
        // a rectangular texture onto a sphere.
        Vector3 topVector(0.0f, +radius, 0.0f); Color topColor(0.0f, +radius, 0.0f, 1.0f); Vector3 topNormal(0.0f, +1.0f, 0.0f); Vector3 topTangent(1.0f, 0.0f, 0.0f); Vector2 topUV(0.0f, 0.0f);
        Vector3 bottomVector(0.0f, -radius, 0.0f); Color bottomColor(0.0f, -radius, 0.0f, 1.0f); Vector3 bottomNormal(0.0f, +1.0f, 0.0f); Vector3 bottomTangent(1.0f, 0.0f, 0.0f); Vector2 bottomUV(0.0f, 1.0f);
        
        Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +radius, 0.0f, 1.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -radius, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        vertices.push_back(topVector);
        colors.push_back(topColor);
        normals.push_back(topNormal);
        tangents.push_back(topTangent);
        uvs.push_back(topUV);

        float phiStep   = M_PI / stackCount;
        float thetaStep = 2.0f * M_PI/ sliceCount;

        // Compute vertices for each stack ring (do not count the poles as rings).
        for(uint32_t i = 1; i <= stackCount-1; ++i)
        {
            float phi = i * phiStep;

            // Vertices of ring.
            for(uint32_t j = 0; j <= sliceCount; ++j)
            {
                float theta = j * thetaStep;

                Vector3 position;
                Color color;
                Vector3 normal;
                Vector3 tangent;
                Vector2 uv;

                // spherical to cartesian
                position.SetX(radius * sinf(phi) * cosf(theta));
                position.SetY(radius * cosf(phi));
                position.SetZ(radius * sinf(phi) * sinf(theta));

                color = Vector4(position.GetX(), position.GetY(), position.GetZ(), 1.0f).Normalized();

                // Partial derivative of P with respect to theta
                tangent.SetX(-radius * sinf(phi) * sinf(theta));
                tangent.SetY(0.0f);
                tangent.SetZ(+radius * sinf(phi) * cosf(theta));

                tangent = tangent.Normalized();

                normal = position.Normalized();

                uv.SetX(theta / (M_PI * 2));
                uv.SetY(phi / M_PI);

                vertices.push_back(position);
                colors.push_back(color);
                normals.push_back(normal);
                tangents.push_back(tangent);
                uvs.push_back(uv);
            }
        }

        vertices.push_back(bottomVector);
        colors.push_back(bottomColor);
        normals.push_back(bottomNormal);
        tangents.push_back(bottomTangent);
        uvs.push_back(bottomUV);

        //
        // Compute indices for top stack.  The top stack was written first to the vertex buffer
        // and connects the top pole to the first ring.
        //

        std::vector<std::uint32_t> indices;
        
        for(std::uint32_t i = 1; i <= sliceCount; ++i)
        {
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(0);
        }

        //
        // Compute indices for inner stacks (not connected to poles).
        //

        // Offset the indices to the index of the first vertex in the first ring.
        // This is just skipping the top pole vertex.
        std::uint32_t baseIndex = 1;
        std::uint32_t ringVertexCount = sliceCount + 1;
        for(std::uint32_t i = 0; i < stackCount-2; ++i)
        {
            for(std::uint32_t j = 0; j < sliceCount; ++j)
            {
                indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
                indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                indices.push_back(baseIndex + i * ringVertexCount + j);

                indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
                indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            }
        }

        //
        // Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
        // and connects the bottom pole to the bottom ring.
        //

        // South pole vertex was added last.
        std::uint32_t southPoleIndex = (std::uint32_t)vertices.size()-1;

        // Offset the indices to the index of the first vertex in the last ring.
        baseIndex = southPoleIndex - ringVertexCount;
        
        for(std::uint32_t i = 0; i < sliceCount; ++i)
        {
            indices.push_back(baseIndex + i + 1);
            indices.push_back(baseIndex + i);
            indices.push_back(southPoleIndex);
        }
        
        meshData.SetVertices(vertices);
        meshData.SetColors(colors);
        meshData.SetNormals(normals);
        meshData.SetTangents(tangents);
        meshData.SetUVs(uvs);
        meshData.SetTriangles(indices);

        return meshData;
    }

    Mesh CreateGeosphere(float radius, std::uint32_t numSubdivisions) {
        Mesh meshData;
        meshData.SetName("Geosphere");
        // Put a cap on the number of subdivisions.
        numSubdivisions = std::min<uint32_t>(numSubdivisions, 6u);

        // Approximate a sphere by tessellating an icosahedron.

        const float X = 0.525731f;
        const float Z = 0.850651f;

        std::vector<Vector3> pos {
            Vector3(-X, 0.0f, Z),  Vector3(X, 0.0f, Z),
            Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
            Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
            Vector3(0.0f, -Z, X),  Vector3(0.0f, -Z, -X),
            Vector3(Z, X, 0.0f),   Vector3(-Z, X, 0.0f),
            Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
        };

        std::vector<uint32_t> k {
            0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
            8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
            7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
            6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
        };

        meshData.SetVertices(pos);
        meshData.SetTriangles(k);

        for(uint32_t i = 0; i < numSubdivisions; ++i)
            Subdivide(meshData);

        // Project vertices onto sphere and scale.
        for(uint32_t i = 0; i < meshData.GetVertices().size(); ++i)
        {
            // Project onto unit sphere.
            Vector3 n = meshData.GetVertices()[i].Normalized();

            // Project onto sphere.
            Vector3 p = radius * n;

            meshData.GetVertices()[i] = p;
            Vector4 color(p.GetX(), p.GetY(), p.GetZ(), 1.0f);
            color.Normalize();
            meshData.GetColors()[i] = Color(color.GetX(), color.GetY(), color.GetZ(), color.GetW());
            meshData.GetNormals()[i] = n;

            float theta = atan2f(meshData.GetVertices()[i].GetZ(), meshData.GetVertices()[i].GetX());

            // Put in [0, 2pi].
            if(theta < 0.0f)
                theta += (M_PI * 2);

            float phi = acosf(meshData.GetVertices()[i].GetY() / radius);

            meshData.GetUVs()[i].SetX(theta / (M_PI * 2));
            meshData.GetUVs()[i].SetY(phi / M_PI);

            // Partial derivative of P with respect to theta
            meshData.GetTangents()[i].SetX(-radius * sinf(phi) * sinf(theta));
            meshData.GetTangents()[i].SetY(0.0f);
            meshData.GetTangents()[i].SetZ(+radius * sinf(phi) * cosf(theta));

            meshData.GetTangents()[i] = meshData.GetTangents()[i].Normalized();
        }

        return meshData;
    }

    Mesh CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount) {
        Mesh meshData;
        meshData.SetName("Cylinder");
        //
        // Build Stacks.
        //

        float stackHeight = height / stackCount;

        // Amount to increment radius as we move up each stack level from bottom to top.
        float radiusStep = (topRadius - bottomRadius) / stackCount;

        std::uint32_t ringCount = stackCount+1;

        // Compute vertices for each stack ring starting at the bottom and moving up.
        for(std::uint32_t i = 0; i < ringCount; ++i)
        {
            float y = -0.5f * height + i * stackHeight;
            float r = bottomRadius + i*radiusStep;

            // vertices of ring
            float dTheta = 2.0f * M_PI / sliceCount;
            for(std::uint32_t j = 0; j <= sliceCount; ++j)
            {
                Vertex vertex;
                
                Vector3 position, tangent, normal;
                Color color;
                Vector2 texCoord;

                float c = cosf(j * dTheta);
                float s = sinf(j * dTheta);

                position = Vector3(r * c, y, r * s);
                Vector4 vecColor(position.GetX(), position.GetY(), position.GetZ(), 1.0f);
                vecColor.Normalize();
                color = Color(vecColor.GetX(), vecColor.GetY(), vecColor.GetZ(), vecColor.GetW());
                texCoord.SetX((float)j / sliceCount);
                texCoord.SetY(1.0f - (float)i / stackCount);

                tangent = Vector3(-s, 0.0f, c);

                float dr = bottomRadius - topRadius;
                Vector3 bitangent(dr * c, -height, dr * s);

                Vector3 T = vertex.tangent;
                Vector3 B(bitangent);
                Vector3 N = Vector3::Cross(T, B).Normalized();
                normal = N;

                meshData.GetVertices().push_back(position);
                meshData.GetColors().push_back(color);
                meshData.GetNormals().push_back(normal);
                meshData.GetTangents().push_back(tangent);
                meshData.GetUVs().push_back(texCoord);
            }
        }

        // Add one because we duplicate the first and last vertex per ring
        // since the texture coordinates are different.
        std::uint32_t ringVertexCount = sliceCount + 1;

        // Compute indices for each stack.
        for(std::uint32_t i = 0; i < stackCount; ++i)
        {
            for(std::uint32_t j = 0; j < sliceCount; ++j)
            {
                meshData.GetTriangles().push_back((i + 1) * ringVertexCount + j + 1);
                meshData.GetTriangles().push_back((i + 1) * ringVertexCount + j);
                meshData.GetTriangles().push_back(i * ringVertexCount + j);

                meshData.GetTriangles().push_back(i * ringVertexCount + j + 1);
                meshData.GetTriangles().push_back((i + 1) * ringVertexCount + j + 1);
                meshData.GetTriangles().push_back(i * ringVertexCount + j);
            }
        }

        BuildCylinderCap(topRadius, TOP, height, sliceCount, stackCount, meshData);
        BuildCylinderCap(bottomRadius, BOTTOM, height, sliceCount, stackCount, meshData);

        return meshData;
    }

    Mesh CreateGrid(float width, float depth, std::uint32_t m, std::uint32_t n) {
        Mesh meshData;
        meshData.SetName("Grid");
        std::uint32_t vertexCount = m * n;
        std::uint32_t faceCount = (m - 1) * (n - 1) * 2;

        //
        // Create the vertices.
        //

        float halfWidth = 0.5f * width;
        float halfDepth = 0.5f * depth;

        float dx = width / (n - 1);
        float dz = depth / (m - 1);

        float du = 1.0f / (n-1);
        float dv = 1.0f / (m-1);

        meshData.GetVertices().resize(vertexCount);
        meshData.GetColors().resize(vertexCount);
        meshData.GetNormals().resize(vertexCount);
        meshData.GetTangents().resize(vertexCount);
        meshData.GetUVs().resize(vertexCount);
        for(uint32_t i = 0; i < m; ++i)
        {
            float z = halfDepth - i*dz;
            for(uint32_t j = 0; j < n; ++j)
            {
                float x = -halfWidth + j*dx;

                meshData.GetVertices()[i * n + j] = Vector3(x, 0.0f, z);
                meshData.GetColors()[i * n + j] = Color(x, 0.0f, z, 1.0f);
                meshData.GetNormals()[i * n + j] = Vector3(0.0f, 1.0f, 0.0f);
                meshData.GetTangents()[i * n + j] = Vector3(1.0f, 0.0f, 0.0f);

                // Stretch texture over grid.
                meshData.GetUVs()[i * n + j].SetX(j * du);
                meshData.GetUVs()[i * n + j].SetY(i * dv);
            }
        }
     
        //
        // Create the indices.
        //

        meshData.GetTriangles().resize(faceCount * 3); // 3 indices per face

        // Iterate over each quad and compute indices.
        // TODO: Indices have to work for culling face back
        uint32_t k = 0;
        for(uint32_t i = 0; i < m - 1; ++i)
        {
            for(uint32_t j = 0; j < n - 1; ++j)
            {
                meshData.GetTriangles()[k] = (i + 1) * n + j;
                meshData.GetTriangles()[k + 1] = i * n + j + 1;
                meshData.GetTriangles()[k + 2] =  i * n + j;
                
                meshData.GetTriangles()[k + 3] = (i + 1 ) * n + j + 1;
                meshData.GetTriangles()[k + 4] = i * n + j + 1;
                meshData.GetTriangles()[k + 5] = (i + 1) * n + j;
                k += 6; // next quad
            }
        }

        return meshData;
    }

    Mesh CreateQuad(float x, float y, float w, float h, float depth) {
        Mesh meshData;
        meshData.SetName("Quad");
        meshData.GetVertices().resize(4);
        meshData.GetColors().resize(4);
        meshData.GetNormals().resize(4);
        meshData.GetTangents().resize(4);
        meshData.GetUVs().resize(4);
        meshData.GetTriangles().resize(6);

        // Position coordinates specified in NDC space.
        meshData.GetVertices()[0]  = Vector3(x, y - h, depth);
        meshData.GetColors()[0]    = Color(1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[0]   = Vector3(0.0f, 0.0f, -1.0f);
        meshData.GetTangents()[0]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[0]       = Vector2(0.0f, 1.0f);

        meshData.GetVertices()[1]  = Vector3(x, y, depth);
        meshData.GetColors()[1]    = Color(1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[1]   = Vector3(0.0f, 0.0f, -1.0f);
        meshData.GetTangents()[1]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[1]       = Vector2(0.0f, 0.0f);

        meshData.GetVertices()[2]  = Vector3(x + w, y, depth);
        meshData.GetColors()[2]    = Color(1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[2]   = Vector3(0.0f, 0.0f, -1.0f);
        meshData.GetTangents()[2]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[2]       = Vector2(1.0f, 0.0f);

        meshData.GetVertices()[3]  = Vector3(x + w, y - h, depth);
        meshData.GetColors()[3]    = Color(1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[3]   = Vector3(0.0f, 0.0f, -1.0f);
        meshData.GetTangents()[3]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[3]       = Vector2(1.0f, 1.0f);

        meshData.GetTriangles()[0] = 0;
        meshData.GetTriangles()[1] = 1;
        meshData.GetTriangles()[2] = 2;

        meshData.GetTriangles()[3] = 0;
        meshData.GetTriangles()[4] = 2;
        meshData.GetTriangles()[5] = 3;

        return meshData;
    }

    Mesh CreateEmpty() {
        Mesh meshData;
        meshData.SetName("Empty");
        meshData.GetVertices().resize(1);
        meshData.GetColors().resize(1);
        meshData.GetNormals().resize(1);
        meshData.GetTangents().resize(1);
        meshData.GetUVs().resize(1);
        meshData.GetTriangles().resize(1);
        

        meshData.GetVertices()[0]  = Vector3(0.0f, 0.0f, 0.0f);
        meshData.GetColors()[0]    = Color  (1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[0]   = Vector3(0.0f, 0.0f, 1.0f);
        meshData.GetTangents()[0]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[0]       = Vector2(1.0f, 1.0f);
        
        meshData.GetTriangles()[0] = 0;
        return meshData;
    }

    Mesh CreateTriangle()
    {
        Mesh meshData;
        meshData.SetName("Triangle");
        meshData.GetVertices().resize(3);
        meshData.GetColors().resize(3);
        meshData.GetNormals().resize(3);
        meshData.GetTangents().resize(3);
        meshData.GetUVs().resize(3);
        meshData.GetTriangles().resize(3);

        meshData.GetVertices()[0]  = Vector3(1.0f, -1.0f, 0.0f);
        meshData.GetColors()[0]    = Color(1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[0]   = Vector3(0.0f, 0.0f, 1.0f);
        meshData.GetTangents()[0]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[0]       = Vector2(1.0f, 1.0f);

        meshData.GetVertices()[1]  = Vector3(-1.0f, -1.0f, 0.0f);
        meshData.GetColors()[1]    = Color(1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[1]   = Vector3(1.0f, 0.0f, 1.0);
        meshData.GetTangents()[1]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[1]       = Vector2(1.0f, 1.0f);

        meshData.GetVertices()[2]  = Vector3(0.0f, 1.0f, 0.0f);
        meshData.GetColors()[2]    = Color  (1.0f, 1.0f, 1.0f, 1.0f);
        meshData.GetNormals()[2]   = Vector3(0.0f, 0.0f, 0.0f);
        meshData.GetTangents()[2]  = Vector3(1.0f, 0.0f, 0.0f);
        meshData.GetUVs()[2]       = Vector2(1.0f, 1.0f);

        meshData.GetTriangles()[0] = 0;
        meshData.GetTriangles()[1] = 1;
        meshData.GetTriangles()[2] = 2;
        return meshData;
    }

    MeshData ShapeHelper::CreateBox(float width, float height, float depth, std::uint32_t numSubdivisions) {
        MeshData meshData;
        meshData.SetName("Box");
        Vertex v[24];

        float w2 = 0.5f*width;
        float h2 = 0.5f*height;
        float d2 = 0.5f*depth;

        // Fill in the front face vertex data.
        v[0] = Vertex(-w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[1] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        v[3] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

        // Fill in the back face vertex data.
        v[4] = Vertex(-w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
        v[5] = Vertex(+w2, -h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[7] = Vertex(-w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        // Fill in the top face vertex data.
        v[8]  = Vertex(-w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[10] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        v[11] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

        // Fill in the bottom face vertex data.
        v[12] = Vertex(-w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
        v[13] = Vertex(+w2, -h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[14] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[15] = Vertex(-w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        // Fill in the left face vertex data.
        v[16] = Vertex(-w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
        v[17] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
        v[18] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
        v[19] = Vertex(-w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

        // Fill in the right face vertex data.
        v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
        v[21] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        v[22] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
        v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

        meshData.vertices.assign(&v[0], &v[24]);

        std::uint32_t i[36];

        // Fill in the front face index data
        i[0] = 2; i[1] = 1; i[2] = 0;
        i[3] = 3; i[4] = 2; i[5] = 0;

        // Fill in the back face index data
        i[6] = 6; i[7]  = 5; i[8]  = 4;
        i[9] = 7; i[10] = 6; i[11] = 4;

        // Fill in the top face index data
        i[12] = 10; i[13] =  9; i[14] = 8;
        i[15] = 11; i[16] = 10; i[17] = 8;

        // Fill in the bottom face index data
        i[18] = 14; i[19] = 13; i[20] = 12;
        i[21] = 15; i[22] = 14; i[23] = 12;

        // Fill in the left face index data
        i[24] = 18; i[25] = 17; i[26] = 16;
        i[27] = 19; i[28] = 18; i[29] = 16;

        // Fill in the right face index data
        i[30] = 22; i[31] = 21; i[32] = 20;
        i[33] = 23; i[34] = 22; i[35] = 20;

        meshData.indices.assign(&i[0], &i[36]);

        numSubdivisions = std::min<std::uint32_t>(numSubdivisions, 6u);
        for(std::uint32_t i = 0; i < numSubdivisions; ++i)
            Subdivide(meshData);

        return meshData;
    }

    MeshData ShapeHelper::CreateSphere(float radius, std::uint32_t sliceCount, std::uint32_t stackCount) {
        MeshData meshData;
        meshData.SetName("Sphere");
        //
        // Compute the vertices stating at the top pole and moving down the stacks.
        //

        // Poles: note that there will be texture coordinate distortion as there is
        // not a unique point on the texture map to assign to the pole when mapping
        // a rectangular texture onto a sphere.
        Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +radius, 0.0f, 1.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -radius, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        meshData.vertices.push_back( topVertex );

        float phiStep   = M_PI / stackCount;
        float thetaStep = 2.0f * M_PI/ sliceCount;

        // Compute vertices for each stack ring (do not count the poles as rings).
        for(uint32_t i = 1; i <= stackCount-1; ++i)
        {
            float phi = i * phiStep;

            // Vertices of ring.
            for(uint32_t j = 0; j <= sliceCount; ++j)
            {
                float theta = j * thetaStep;

                Vertex v;

                // spherical to cartesian
                v.position.SetX(radius * sinf(phi) * cosf(theta));
                v.position.SetY(radius * cosf(phi));
                v.position.SetZ(radius * sinf(phi) * sinf(theta));

                Vector3 pos = v.position;
                v.color = Vector4(pos.GetX(), pos.GetY(), pos.GetZ(), 1.0f).Normalized();

                // Partial derivative of P with respect to theta
                v.tangent.SetX(-radius * sinf(phi) * sinf(theta));
                v.tangent.SetY(0.0f);
                v.tangent.SetZ(+radius * sinf(phi) * cosf(theta));

                v.tangent = v.tangent.Normalized();

                v.normal = v.position.Normalized();

                v.texCoord.SetX(theta / (M_PI * 2));
                v.texCoord.SetY(phi / M_PI);

                meshData.vertices.push_back( v );
            }
        }

        meshData.vertices.push_back( bottomVertex );

        //
        // Compute indices for top stack.  The top stack was written first to the vertex buffer
        // and connects the top pole to the first ring.
        //

        for(std::uint32_t i = 1; i <= sliceCount; ++i)
        {
            meshData.indices.push_back(i);
            meshData.indices.push_back(i + 1);
            meshData.indices.push_back(0);
        }

        //
        // Compute indices for inner stacks (not connected to poles).
        //

        // Offset the indices to the index of the first vertex in the first ring.
        // This is just skipping the top pole vertex.
        std::uint32_t baseIndex = 1;
        std::uint32_t ringVertexCount = sliceCount + 1;
        for(std::uint32_t i = 0; i < stackCount-2; ++i)
        {
            for(std::uint32_t j = 0; j < sliceCount; ++j)
            {
                meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
                meshData.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                meshData.indices.push_back(baseIndex + i * ringVertexCount + j);

                meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
                meshData.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);                
            }
        }

        //
        // Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
        // and connects the bottom pole to the bottom ring.
        //

        // South pole vertex was added last.
        std::uint32_t southPoleIndex = (std::uint32_t)meshData.vertices.size()-1;

        // Offset the indices to the index of the first vertex in the last ring.
        baseIndex = southPoleIndex - ringVertexCount;
        
        for(std::uint32_t i = 0; i < sliceCount; ++i)
        {
            meshData.indices.push_back(baseIndex + i + 1);
            meshData.indices.push_back(baseIndex + i);
            meshData.indices.push_back(southPoleIndex);
        }

        return meshData;
    }

    MeshData ShapeHelper::CreateGeosphere(float radius, std::uint32_t numSubdivisions) {
        MeshData meshData;
        meshData.SetName("Geosphere");
        // Put a cap on the number of subdivisions.
        numSubdivisions = std::min<uint32_t>(numSubdivisions, 6u);

        // Approximate a sphere by tessellating an icosahedron.

        const float X = 0.525731f;
        const float Z = 0.850651f;

        Vector3 pos[12] =
        {
            Vector3(-X, 0.0f, Z),  Vector3(X, 0.0f, Z),
            Vector3(-X, 0.0f, -Z), Vector3(X, 0.0f, -Z),
            Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
            Vector3(0.0f, -Z, X),  Vector3(0.0f, -Z, -X),
            Vector3(Z, X, 0.0f),   Vector3(-Z, X, 0.0f),
            Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
        };

        uint32_t k[60] =
        {
            0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
            8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
            7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
            6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
        };

        meshData.vertices.resize(12);
        meshData.indices.assign(&k[0], &k[60]);

        for(uint32_t i = 0; i < 12; ++i)
            meshData.vertices[i].position = pos[i];

        for(uint32_t i = 0; i < numSubdivisions; ++i)
            Subdivide(meshData);

        // Project vertices onto sphere and scale.
        for(uint32_t i = 0; i < meshData.vertices.size(); ++i)
        {
            // Project onto unit sphere.
            Vector3 n = meshData.vertices[i].position.Normalized();

            // Project onto sphere.
            Vector3 p = radius * n;

            meshData.vertices[i].position = p;
            meshData.vertices[i].color = Vector4(p.GetX(), p.GetY(), p.GetZ(), 1.0f).Normalized();
            meshData.vertices[i].normal = n;

            float theta = atan2f(meshData.vertices[i].position.GetZ(), meshData.vertices[i].position.GetX());

            // Put in [0, 2pi].
            if(theta < 0.0f)
                theta += (M_PI * 2);

            float phi = acosf(meshData.vertices[i].position.GetY() / radius);

            meshData.vertices[i].texCoord.GetX() = theta / (M_PI * 2);
            meshData.vertices[i].texCoord.GetY() = phi / M_PI;

            // Partial derivative of P with respect to theta
            meshData.vertices[i].tangent.SetX(-radius * sinf(phi) * sinf(theta));
            meshData.vertices[i].tangent.SetY(0.0f);
            meshData.vertices[i].tangent.SetZ(+radius * sinf(phi) * cosf(theta));

            meshData.vertices[i].tangent = meshData.vertices[i].tangent.Normalized();
        }

        return meshData;
    }

    MeshData ShapeHelper::CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount) {
        MeshData meshData;
        meshData.SetName("Cylinder");
        //
        // Build Stacks.
        //

        float stackHeight = height / stackCount;

        // Amount to increment radius as we move up each stack level from bottom to top.
        float radiusStep = (topRadius - bottomRadius) / stackCount;

        std::uint32_t ringCount = stackCount+1;

        // Compute vertices for each stack ring starting at the bottom and moving up.
        for(std::uint32_t i = 0; i < ringCount; ++i)
        {
            float y = -0.5f * height + i * stackHeight;
            float r = bottomRadius + i*radiusStep;

            // vertices of ring
            float dTheta = 2.0f * M_PI / sliceCount;
            for(std::uint32_t j = 0; j <= sliceCount; ++j)
            {
                Vertex vertex;

                float c = cosf(j * dTheta);
                float s = sinf(j * dTheta);

                vertex.position = Vector3(r * c, y, r * s);
                vertex.color = Vector4(vertex.position.GetX(), vertex.position.GetY(), vertex.position.GetZ(), 1.0f).Normalized();
                vertex.texCoord.SetX((float)j / sliceCount);
                vertex.texCoord.SetY(1.0f - (float)i / stackCount);

                vertex.tangent = Vector3(-s, 0.0f, c);

                float dr = bottomRadius - topRadius;
                Vector3 bitangent(dr * c, -height, dr * s);

                Vector3 T = vertex.tangent;
                Vector3 B(bitangent);
                Vector3 N = Vector3::Cross(T, B).Normalized();
                vertex.normal = N;

                meshData.vertices.push_back(vertex);
            }
        }

        // Add one because we duplicate the first and last vertex per ring
        // since the texture coordinates are different.
        std::uint32_t ringVertexCount = sliceCount + 1;

        // Compute indices for each stack.
        for(std::uint32_t i = 0; i < stackCount; ++i)
        {
            for(std::uint32_t j = 0; j < sliceCount; ++j)
            {
                meshData.indices.push_back((i + 1) * ringVertexCount + j + 1);
                meshData.indices.push_back((i + 1) * ringVertexCount + j);
                meshData.indices.push_back(i * ringVertexCount + j);

                meshData.indices.push_back(i * ringVertexCount + j + 1);
                meshData.indices.push_back((i + 1) * ringVertexCount + j + 1);
                meshData.indices.push_back(i * ringVertexCount + j);
            }
        }

        BuildCylinderCap(topRadius, TOP, height, sliceCount, stackCount, meshData);
        BuildCylinderCap(bottomRadius, BOTTOM, height, sliceCount, stackCount, meshData);

        return meshData;
    }

    MeshData ShapeHelper::CreateGrid(float width, float depth, std::uint32_t m, std::uint32_t n) {
        MeshData meshData;
        meshData.SetName("Grid");
        std::uint32_t vertexCount = m * n;
        std::uint32_t faceCount = (m - 1) * (n - 1) * 2;

        //
        // Create the vertices.
        //

        float halfWidth = 0.5f * width;
        float halfDepth = 0.5f * depth;

        float dx = width / (n - 1);
        float dz = depth / (m - 1);

        float du = 1.0f / (n-1);
        float dv = 1.0f / (m-1);

        meshData.vertices.resize(vertexCount);
        for(uint32_t i = 0; i < m; ++i)
        {
            float z = halfDepth - i*dz;
            for(uint32_t j = 0; j < n; ++j)
            {
                float x = -halfWidth + j*dx;

                meshData.vertices[i * n + j].position = Vector3(x, 0.0f, z);
                meshData.vertices[i * n + j].color = Vector4(x, 0.0f, z, 1.0f);
                meshData.vertices[i * n + j].normal = Vector3(0.0f, 1.0f, 0.0f);
                meshData.vertices[i * n + j].tangent = Vector3(1.0f, 0.0f, 0.0f);

                // Stretch texture over grid.
                meshData.vertices[i * n + j].texCoord.SetX(j * du);
                meshData.vertices[i * n + j].texCoord.SetY(i * dv);
            }
        }
     
        //
        // Create the indices.
        //

        meshData.indices.resize(faceCount * 3); // 3 indices per face

        // Iterate over each quad and compute indices.
        // TODO: Indices have to work for culling face back
        uint32_t k = 0;
        for(uint32_t i = 0; i < m - 1; ++i)
        {
            for(uint32_t j = 0; j < n - 1; ++j)
            {
                meshData.indices[k] = (i + 1) * n + j;
                meshData.indices[k + 1] = i * n + j + 1;
                meshData.indices[k + 2] =  i * n + j;
                
                meshData.indices[k + 3] = (i + 1 ) * n + j + 1;
                meshData.indices[k + 4] = i * n + j + 1;
                meshData.indices[k + 5] = (i + 1) * n + j;
                k += 6; // next quad
            }
        }

        return meshData;
    }

    MeshData ShapeHelper::CreateQuad(float x, float y, float w, float h, float depth) {
        MeshData meshData;
        meshData.SetName("Quad");
        meshData.vertices.resize(4);
        meshData.indices.resize(6);

        // Position coordinates specified in NDC space.
        meshData.vertices[0] = Vertex(x, y - h, depth,
                                      1.0f, 1.0f, 1.0f, 1.0f,
                                      0.0f, 0.0f, -1.0f,
                                      1.0f, 0.0f, 0.0f,
                                      0.0f, 1.0f);

        meshData.vertices[1] = Vertex(x, y, depth,
                                      1.0f, 1.0f, 1.0f, 1.0f,
                                      0.0f, 0.0f, -1.0f,
                                      1.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f);

        meshData.vertices[2] = Vertex(x + w, y, depth,
                                      1.0f, 1.0f, 1.0f, 1.0f,
                                      0.0f, 0.0f, -1.0f,
                                      1.0f, 0.0f, 0.0f,
                                      1.0f, 0.0f);

        meshData.vertices[3] = Vertex(x + w, y - h, depth,
                                      1.0f, 1.0f, 1.0f, 1.0f,
                                      0.0f, 0.0f, -1.0f,
                                      1.0f, 0.0f, 0.0f,
                                      1.0f, 1.0f);

        meshData.indices[0] = 0;
        meshData.indices[1] = 1;
        meshData.indices[2] = 2;

        meshData.indices[3] = 0;
        meshData.indices[4] = 2;
        meshData.indices[5] = 3;

        return meshData;
    }

    MeshData ShapeHelper::CreateEmpty() {
        MeshData meshData;
        meshData.SetName("Empty");
        meshData.vertices.resize(1);
        meshData.indices.resize(1);
        

        meshData.vertices[0] = Vertex(0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f);
        meshData.indices[0] = 0;
        return meshData;
    }

    MeshData ShapeHelper::CreateTriangle()
    {
        MeshData meshData;
        meshData.SetName("Triangle");
        meshData.vertices.resize(3);
        meshData.indices.resize(3);

        meshData.vertices[0] = Vertex(1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f);

        meshData.vertices[1] = Vertex(-1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f);

        meshData.vertices[2] = Vertex(0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f);

        meshData.indices[0] = 0;
        meshData.indices[1] = 1;
        meshData.indices[2] = 2;
        return meshData;
    }

    void ShapeHelper::Subdivide(MeshData& meshData) {
        MeshData inputCopy = meshData;


        meshData.vertices.resize(0);
        meshData.indices.resize(0);

        //       v1
        //       *
        //      / \
        //     /   \
        //  m0*-----*m1
        //   / \   / \
        //  /   \ /   \
        // *-----*-----*
        // v0    m2     v2

        uint32_t numTris = (uint32_t)inputCopy.indices.size()/3;
        for(uint32_t i = 0; i < numTris; ++i)
        {
            Vertex v0 = inputCopy.vertices[ inputCopy.indices[i * 3 + 0] ];
            Vertex v1 = inputCopy.vertices[ inputCopy.indices[i * 3 + 1] ];
            Vertex v2 = inputCopy.vertices[ inputCopy.indices[i * 3 + 2] ];

            //
            // Generate the midpoints.
            //

            Vertex m0 = MidPoint(v0, v1);
            Vertex m1 = MidPoint(v1, v2);
            Vertex m2 = MidPoint(v0, v2);

            //
            // Add new geometry.
            //

            meshData.vertices.push_back(v0); // 0
            meshData.vertices.push_back(v1); // 1
            meshData.vertices.push_back(v2); // 2
            meshData.vertices.push_back(m0); // 3
            meshData.vertices.push_back(m1); // 4
            meshData.vertices.push_back(m2); // 5
     
            meshData.indices.push_back(i * 6 + 0);
            meshData.indices.push_back(i * 6 + 3);
            meshData.indices.push_back(i * 6 + 5);

            meshData.indices.push_back(i * 6 + 3);
            meshData.indices.push_back(i * 6 + 4);
            meshData.indices.push_back(i * 6 + 5);

            meshData.indices.push_back(i * 6 + 5);
            meshData.indices.push_back(i * 6 + 4);
            meshData.indices.push_back(i * 6 + 2);

            meshData.indices.push_back(i * 6 + 3);
            meshData.indices.push_back(i * 6 + 1);
            meshData.indices.push_back(i * 6 + 4);
        }
    }

    Vertex ShapeHelper::MidPoint(const Vertex& v0, const Vertex& v1) {
        Vector3 pos = Midpoint(v0.position, v1.position);
        Vector4 color = Midpoint(v0.color, v1.color);
        Vector3 normal = NormalizedMidpoint(v0.normal, v1.normal);
        Vector3 tangent = NormalizedMidpoint(v0.tangent, v1.tangent);
        Vector2 texCoord = Midpoint(v0.texCoord, v1.texCoord);
        return Vertex(pos, color, normal, tangent, texCoord);
    }

    void ShapeHelper::BuildCylinderCap(float radius, int cap, float height, std::uint32_t sliceCount, std::uint32_t stackCount, MeshData& meshData) {
        if (cap != TOP && cap != BOTTOM) return;
        std::uint32_t baseIndex = (std::uint32_t)meshData.vertices.size();
        float y = (0.5f * cap) * height;

        // vertices of ring
        float dTheta = 2.0f * M_PI / sliceCount;
        for(std::uint32_t i = 0; i <= sliceCount; ++i)
        {
            float x = radius * cosf(i * dTheta);
            float z = radius * sinf(i * dTheta);
    
            // Scale down by the height to try and make top cap texture coord area
            // proportional to base.
            float u = x / height + 0.5f;
            float v = z / height + 0.5f;

            meshData.vertices.push_back(Vertex(x, y, z, x, y, z, 1.0f, 0.0f, 1.0f * (cap == BOTTOM ? -1: 1), 0.0f, 1.0f, 0.0f, 0.0f, u, v) );
        }

        // Cap center vertex.
        meshData.vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f * (cap == BOTTOM ? -1: 1), 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f) );

        // Cache the index of center vertex.
        std::uint32_t centerIndex = (std::uint32_t)meshData.vertices.size() - 1;

        for(std::uint32_t i = 0; i < sliceCount; ++i)
        {
            if (cap == TOP) {
                meshData.indices.push_back(baseIndex + i);
                meshData.indices.push_back(baseIndex + i + 1);
                meshData.indices.push_back(centerIndex);
            } else if (cap == BOTTOM) {
                meshData.indices.push_back(baseIndex + i + 1);
                meshData.indices.push_back(baseIndex + i);
                meshData.indices.push_back(centerIndex);
            }
            
        }
    }
}
