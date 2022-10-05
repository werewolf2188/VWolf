//
//  ShapeHelper.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/4/22.
//

#pragma once
#include "VWolf/Core/Math/Math.h"
#ifdef VWOLF_PLATFORM_MACOS
#include <vector>
#endif

namespace VWolf {   

    struct Vertex {
        Vertex() {
            position = Vector3Float(0, 0, 0);
            color = Vector4Float(0, 0, 0, 1);
        }

        Vertex(float x, float y, float z, float r, float g, float b, float a) {
            position = Vector3Float(x, y, z);
            color = Vector4Float(r, g, b, a);
        }

        Vertex(Vector3Float pos, Vector4Float col) {
            position = pos;
            color = col;
        }
        Vector3Float position;
        Vector4Float color;
    };

    struct MeshData {
    public:
        std::vector<float> GetVertices();
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
    };

    class ShapeHelper {
    public:
        static MeshData CreateBox(float width, float height, float depth, std::uint32_t numSubdivisions);
        static MeshData CreateSphere(float radius, std::uint32_t sliceCount, std::uint32_t stackCount);
        static MeshData CreateGeosphere(float radius, std::uint32_t numSubdivisions);
        static MeshData CreateCylinder(float bottomRadius, float topRadius, float height, std::uint32_t sliceCount, std::uint32_t stackCount);
        static MeshData CreateGrid(float width, float depth, std::uint32_t m, std::uint32_t n);
        static MeshData CreateQuad(float x, float y, float w, float h, float depth);
    private:
    private:
        static void Subdivide(MeshData& meshData);
        static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
        static void BuildCylinderCap(float radius, int cap, float height, std::uint32_t sliceCount, std::uint32_t stackCount, MeshData& meshData);
    };
}

/* ShapeHelper_h */
