//
//  ShapeHelper.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/4/22.
//

#pragma once
#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Render/RenderStructs.h"
#include "VWolf/Core/Render/Mesh.h"
#ifdef VWOLF_PLATFORM_MACOS
#include <vector>
#endif

#ifndef VWOLF_GET_SHAPE_NAME
#define VWOLF_GET_SHAPE_NAME(n) std::get<0>(n)
#endif
#ifndef GET_SHAPE_ID
#define GET_SHAPE_ID(n) std::get<1>(n)
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
        static MeshData Create(std::string name);
    public:
        static std::tuple<std::string, UUID> Box;
        static std::tuple<std::string, UUID> Sphere;
        static std::tuple<std::string, UUID> Geosphere;
        static std::tuple<std::string, UUID> Cylinder;
        static std::tuple<std::string, UUID> Grid;
        static std::tuple<std::string, UUID> Quad;
        static std::tuple<std::string, UUID> Triangle;
        static std::tuple<std::string, UUID> Monkey;
    public:
//        static Mesh CreateBox(float width, float height, float depth, std::uint32_t numSubdivisions);
//        static Mesh CreateSphere(float radius, std::uint32_t sliceCount, std::uint32_t stackCount);
//        static Mesh CreateGeosphere(float radius, std::uint32_t numSubdivisions);
//        static Mesh CreateCylinder(float bottomRadius, float topRadius, float height, std::uint32_t sliceCount, std::uint32_t stackCount);
//        static Mesh CreateGrid(float width, float depth, std::uint32_t m, std::uint32_t n);
//        static Mesh CreateQuad(float x, float y, float w, float h, float depth);
//        static Mesh CreateTriangle();
//        static Mesh CreateEmpty();
//        static Mesh Create(const char* name);
    private:
    private:
        static void Subdivide(MeshData& meshData);
        static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
        static void BuildCylinderCap(float radius, int cap, float height, std::uint32_t sliceCount, std::uint32_t stackCount, MeshData& meshData);
    };
}

/* ShapeHelper_h */
