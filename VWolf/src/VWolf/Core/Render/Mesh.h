//
//  Mesh.h
//  VWolf
//
//  Created by Enrique Moises on 6/17/26.
//

#pragma once

#include "VWolf/Core/Object.h"
#include "VWolf/Core/Math/VMath.h"

/*
 To get the bounds from a mesh, you typically calculate an Axis-Aligned Bounding Box (AABB) by finding the minimum and maximum coordinates of all vertices along the X, Y, and Z axes.General Algorithm (Pseudocode)Initialize minX, minY, minZ to positive infinity.Initialize maxX, maxY, maxZ to negative infinity.For each vertex in the mesh:Update the min values if the current vertex's coordinates are smaller.Update the max values if the current vertex's coordinates are larger.The final min and max points define the corners of your bounding box.
 */

namespace objl {
    class Loader;
}

namespace VWolf {

    enum class Topology {
        Unknown,
        Triangles,
        Quads,
        Lines,
        LinesStrip,
        Points
    };

    enum class Attribute {
        Unknown,
        Position,
        Normal,
        Tangent,
        Color,
        TextCoord0,
        TextCoord1,
        TextCoord2,
        TextCoord3,
        TextCoord4,
        TextCoord5,
        TextCoord6,
        TextCoord7,
        BlendWeight,
        BlendIndices
    };

    enum class AttributeFormat {
        Unknown,
        Float32,
        Float16,
        UNorm8,
        SNorm8,
        UNorm16,
        SNorm16,
        UInt8,
        SInt8,
        UInt16,
        SInt16,
        UInt32,
        SInt32
    };

    enum class IndexFormat {
        UInt16,
        UInt32
    };

    struct AttributeDescriptor {
    public:
        AttributeDescriptor(): dimension(-1) {}
        AttributeDescriptor(Attribute attribute, AttributeFormat format, uint32_t dimension):
        attribute(attribute), format(format), dimension(dimension) {}
    public:
        const Attribute& GetAttribute() { return attribute; }
        const AttributeFormat& GetFormat() { return format; }
        const uint32_t& GetDimension() { return dimension; }
    private:
        std::string name;
        uint32_t index;
        uint32_t offset;
        Attribute attribute;
        AttributeFormat format;
        uint32_t dimension;
        uint32_t size;
        
        friend class HLSLOpenGLShader;
        friend class OpenGLVertexArray;
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        friend class HLSLMetalShader;
#endif
#ifdef VWOLF_PLATFORM_WINDOWS
        friend class HLSLShader;
        friend class DirectX12VertexBuffer;
#endif
    };

    struct SubMeshDescriptor {
    public:
        SubMeshDescriptor():
        baseVertex(-1),
        firstVertex(-1),
        indexCount(-1),
        indexStart(-1),
        topology(Topology::Triangles),
        vertexCount(-1) { }
        
        SubMeshDescriptor(
                          uint32_t baseVertex,
                          uint32_t firstVertex,
                          size_t indexCount,
                          uint32_t indexStart,
                          Topology topology,
                          size_t vertexCount
                          ):
        baseVertex(baseVertex),
        firstVertex(firstVertex),
        indexCount(indexCount),
        indexStart(indexStart),
        topology(topology),
        vertexCount(vertexCount) { }
    public:
        const uint32_t& GetBaseVertex() const { return baseVertex; }
        const uint32_t& GetFirstVertex() const { return firstVertex; }
        const size_t& GetIndexCount() const { return indexCount; }
        const uint32_t& GetIndexStart() const { return indexStart; }
        const Topology& GetTopology() const { return topology; }
        const size_t& GetVertextCount() const { return vertexCount; }
    private:
        uint32_t baseVertex;
        uint32_t firstVertex;
        size_t indexCount;
        uint32_t indexStart;
        Topology topology;
        size_t vertexCount;
    };

    class Mesh: public Object {
    public:
        Mesh();
        Mesh(UUID id);
        Mesh(objl::Loader& loader, UUID id);
        
        Mesh(const Mesh& mesh);
    public:
        Mesh& operator=(const Mesh& mesh);
    public:
        void RecalculateNormals();
        void RecalculateTangents();
    public:
        const std::vector<Vector3>& GetVertices() const { return vertices; }
        const std::vector<Color>& GetColors() const { return colors; }
        const std::vector<Vector3>& GetNormals() const { return normals; }
        const std::vector<Vector3>& GetTangents() const { return tangents; }
        const std::vector<Vector3>& GetBitangents() const { return bitangents; }
        const std::vector<Vector2>& GetUVs() const { return uvs; }
        
        const std::vector<uint32_t>& GetTriangles() const { return triangles; }
        
        const std::vector<float>& GetNativeVector() { return vertexArray; };
        
        const SubMeshDescriptor& GetSubMesh(uint32_t index) { return subMesh[index]; }
    public:
        std::vector<Vector3>& GetVertices() { return vertices; }
        std::vector<Color>& GetColors() { return colors; }
        std::vector<Vector3>& GetNormals() { return normals; }
        std::vector<Vector3>& GetTangents() { return tangents; }
        std::vector<Vector3>& GetBitangents() { return bitangents; }
        std::vector<Vector2>& GetUVs() { return uvs; }
        
        std::vector<uint32_t>& GetTriangles() { return triangles; }
    public:
        void SetName(std::string _name) { name = _name; }
        
        void SetVertices(std::vector<Vector3> _vertices) { vertices = _vertices; }
        void SetColors(std::vector<Color> _colors) { colors = _colors; }
        void SetNormals(std::vector<Vector3> _normals) { normals = _normals; }
        void SetTangents(std::vector<Vector3> _tangents) { tangents = _tangents; }
        void SetBitangents(std::vector<Vector3> _bitangents) { bitangents = _bitangents; }
        void SetUVs(std::vector<Vector2> _uvs) { uvs = _uvs; }
        
        void SetTriangles(std::vector<uint32_t> _triangles) { triangles = _triangles; }
    public:
        void BuildVertexBuffer(std::vector<AttributeDescriptor> descriptor);
        void Reset();
    public:
        static Ref<Mesh> Load(std::filesystem::path path, UUID id);
    private:
        std::vector<Vector3> vertices;
        std::vector<Color> colors;
        std::vector<Vector3> normals;
        std::vector<Vector3> tangents;
        std::vector<Vector3> bitangents;
        std::vector<Vector2> uvs;
        
        std::vector<uint32_t> triangles;
        
        std::vector<SubMeshDescriptor> subMesh;
        
        std::vector<float> vertexArray;
    };
}
