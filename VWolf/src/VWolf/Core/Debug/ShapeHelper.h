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
#ifndef VWOLF_GET_SHAPE_ID
#define VWOLF_GET_SHAPE_ID(n) std::get<1>(n)
#endif

namespace VWolf {   
    class ShapeHelper {
    public:
        static Mesh CreateBox(float width, float height, float depth, std::uint32_t numSubdivisions);
        static Mesh CreateSphere(float radius, std::uint32_t sliceCount, std::uint32_t stackCount);
        static Mesh CreateGeosphere(float radius, std::uint32_t numSubdivisions);
        static Mesh CreateCylinder(float bottomRadius, float topRadius, float height, std::uint32_t sliceCount, std::uint32_t stackCount);
        static Mesh CreateGrid(float width, float depth, std::uint32_t m, std::uint32_t n);
        static Mesh CreateQuad(float x, float y, float w, float h, float depth);
        static Mesh CreateTriangle();
        static Mesh CreateEmpty();
        static Mesh Create(std::string name);
        static Mesh Create(UUID id);
    public:
        static std::tuple<std::string, UUID> Box;
        static std::tuple<std::string, UUID> Sphere;
        static std::tuple<std::string, UUID> Geosphere;
        static std::tuple<std::string, UUID> Cylinder;
        static std::tuple<std::string, UUID> Grid;
        static std::tuple<std::string, UUID> Quad;
        static std::tuple<std::string, UUID> Triangle;
        static std::tuple<std::string, UUID> Empty;
    };
}

/* ShapeHelper_h */
