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

#ifdef VWOLF_PLATFORM_WINDOWS
# define M_PI           3.14159265358979323846  /* pi */
#endif

namespace VWolf {
    MeshData ShapeHelper::CreateBox(float width, float height, float depth, std::uint32_t numSubdivisions) {
        MeshData meshData;
        Vertex v[24];

        float w2 = 0.5f*width;
        float h2 = 0.5f*height;
        float d2 = 0.5f*depth;

        // Fill in the front face vertex data.
        v[0] = Vertex(-w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f);
        v[1] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f);
        v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, 1.0f, 1.0f);
        v[3] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f);

        // Fill in the back face vertex data.
        v[4] = Vertex(-w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f);
        v[5] = Vertex(+w2, -h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f);
        v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f);
        v[7] = Vertex(-w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f);

        // Fill in the top face vertex data.
        v[8]  = Vertex(-w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f);
        v[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f);
        v[10] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f);
        v[11] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f);

        // Fill in the bottom face vertex data.
        v[12] = Vertex(-w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f);
        v[13] = Vertex(+w2, -h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f);
        v[14] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f);
        v[15] = Vertex(-w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f);

        // Fill in the left face vertex data.
        v[16] = Vertex(-w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f);
        v[17] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f);
        v[18] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, 1.0f, 1.0f);
        v[19] = Vertex(-w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f);

        // Fill in the right face vertex data.
        v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 1.0f);
        v[21] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f);
        v[22] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, 1.0f);
        v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 1.0f);

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

        //
        // Compute the vertices stating at the top pole and moving down the stacks.
        //

        // Poles: note that there will be texture coordinate distortion as there is
        // not a unique point on the texture map to assign to the pole when mapping
        // a rectangular texture onto a sphere.
        Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +radius, 0.0f, 1.0f);
        Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -radius, 0.0f, 1.0f);

        meshData.vertices.push_back( topVertex );

        float phiStep   = M_PI / stackCount;
        float thetaStep = 2.0f * M_PI/ sliceCount;

        // Compute vertices for each stack ring (do not count the poles as rings).
        for(uint32 i = 1; i <= stackCount-1; ++i)
        {
            float phi = i * phiStep;

            // Vertices of ring.
            for(uint32 j = 0; j <= sliceCount; ++j)
            {
                float theta = j * thetaStep;

                Vertex v;

                // spherical to cartesian
                v.position.x = radius * sinf(phi) * cosf(theta);
                v.position.y = radius * cosf(phi);
                v.position.z = radius * sinf(phi) * sinf(theta);
                
                Vector3Float pos = v.position;
                v.color = normalize(Vector4Float(pos.x, pos.y, pos.z, 1.0f));

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

        // Put a cap on the number of subdivisions.
        numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

        // Approximate a sphere by tessellating an icosahedron.

        const float X = 0.525731f;
        const float Z = 0.850651f;

        Vector3Float pos[12] =
        {
            Vector3Float(-X, 0.0f, Z),  Vector3Float(X, 0.0f, Z),
            Vector3Float(-X, 0.0f, -Z), Vector3Float(X, 0.0f, -Z),
            Vector3Float(0.0f, Z, X),   Vector3Float(0.0f, Z, -X),
            Vector3Float(0.0f, -Z, X),  Vector3Float(0.0f, -Z, -X),
            Vector3Float(Z, X, 0.0f),   Vector3Float(-Z, X, 0.0f),
            Vector3Float(Z, -X, 0.0f),  Vector3Float(-Z, -X, 0.0f)
        };

        uint32 k[60] =
        {
            0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
            8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
            7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
            6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
        };

        meshData.vertices.resize(12);
        meshData.indices.assign(&k[0], &k[60]);

        for(uint32 i = 0; i < 12; ++i)
            meshData.vertices[i].position = pos[i];

        for(uint32 i = 0; i < numSubdivisions; ++i)
            Subdivide(meshData);

        // Project vertices onto sphere and scale.
        for(uint32 i = 0; i < meshData.vertices.size(); ++i)
        {
            // Project onto unit sphere.
            Vector3Float n = normalize(meshData.vertices[i].position);

            // Project onto sphere.
            Vector3Float p = radius*n;

            meshData.vertices[i].position = p;
            meshData.vertices[i].color = normalize(Vector4Float(p.x, p.y, p.z, 1.0f));
        }

        return meshData;
    }

    MeshData ShapeHelper::CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount) {
        MeshData meshData;

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

                vertex.position = Vector3Float(r * c, y, r * s);
                vertex.color = normalize(Vector4Float(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f));
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

        std::uint32_t vertexCount = m * n;
        std::uint32_t faceCount = (m - 1) * (n - 1) * 2;

        //
        // Create the vertices.
        //

        float halfWidth = 0.5f * width;
        float halfDepth = 0.5f * depth;

        float dx = width / (n - 1);
        float dz = depth / (m - 1);

        meshData.vertices.resize(vertexCount);
        for(uint32 i = 0; i < m; ++i)
        {
            float z = halfDepth - i*dz;
            for(uint32 j = 0; j < n; ++j)
            {
                float x = -halfWidth + j*dx;

                meshData.vertices[i * n + j].position = Vector3Float(x, 0.0f, z);
                meshData.vertices[i * n + j].color = Vector4Float(x, 0.0f, z, 1.0f);
            }
        }
     
        //
        // Create the indices.
        //

        meshData.indices.resize(faceCount * 3); // 3 indices per face

        // Iterate over each quad and compute indices.
        // TODO: Indices have to work for culling face back
        uint32 k = 0;
        for(uint32 i = 0; i < m - 1; ++i)
        {
            for(uint32 j = 0; j < n - 1; ++j)
            {
                meshData.indices[k + 2] = (i + 1) * n + j;
                meshData.indices[k + 1] = i * n + j + 1;
                meshData.indices[k]   = i * n + j;

                meshData.indices[k + 5] = (i + 1 ) * n + j + 1;
                meshData.indices[k + 4] = i * n + j + 1;
                meshData.indices[k + 3] = (i + 1) * n + j;

                k += 6; // next quad
            }
        }

        return meshData;
    }

    MeshData ShapeHelper::CreateQuad(float x, float y, float w, float h, float depth) {
        MeshData meshData;

        meshData.vertices.resize(4);
        meshData.indices.resize(6);

        // Position coordinates specified in NDC space.
        meshData.vertices[0] = Vertex(x, y - h, depth, 1.0f, 1.0f, 1.0f, 1.0f);

        meshData.vertices[1] = Vertex(x, y, depth, 1.0f, 1.0f, 1.0f, 1.0f);

        meshData.vertices[2] = Vertex(x + w, y, depth, 1.0f, 1.0f, 1.0f, 1.0f);

        meshData.vertices[3] = Vertex(x + w, y - h, depth, 1.0f, 1.0f, 1.0f, 1.0f);

        meshData.indices[0] = 0;
        meshData.indices[1] = 1;
        meshData.indices[2] = 2;

        meshData.indices[3] = 0;
        meshData.indices[4] = 2;
        meshData.indices[5] = 3;

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

        uint32 numTris = (uint32)inputCopy.indices.size()/3;
        for(uint32 i = 0; i < numTris; ++i)
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
        Vector3Float pos = 0.5f * (v0.position + v1.position);
        Vector4Float color = 0.5f * (v0.color + v1.color);
        return Vertex(pos, color);
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

            meshData.vertices.push_back(Vertex(x, y, z, x, y, z, 1.0f) );
        }

        // Cap center vertex.
        meshData.vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f) );

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
