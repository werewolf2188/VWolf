//
//  ShapeHelper.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/4/22.
//

#pragma once
#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Render/RenderStructs.h"
#ifdef VWOLF_PLATFORM_MACOS
#include <vector>
#endif

namespace VWolf {   
    class ShapeHelper {
    public:
        static MeshData CreateBox(float width, float height, float depth, std::uint32_t numSubdivisions);
        static MeshData CreateSphere(float radius, std::uint32_t sliceCount, std::uint32_t stackCount);
        static MeshData CreateGeosphere(float radius, std::uint32_t numSubdivisions);
        static MeshData CreateCylinder(float bottomRadius, float topRadius, float height, std::uint32_t sliceCount, std::uint32_t stackCount);
        static MeshData CreateGrid(float width, float depth, std::uint32_t m, std::uint32_t n);
        static MeshData CreateQuad(float x, float y, float w, float h, float depth);
        static MeshData CreateTriangle();
        static MeshData CreateEmpty();
        static MeshData Create(const char* name);
    private:
    private:
        static void Subdivide(MeshData& meshData);
        static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
        static void BuildCylinderCap(float radius, int cap, float height, std::uint32_t sliceCount, std::uint32_t stackCount, MeshData& meshData);
    };
}

/* ShapeHelper_h */
