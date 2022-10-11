//
//  VertexInfo.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once
#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/Math.h"
#ifdef VWOLF_PLATFORM_MACOS
#include <vector>
#endif

namespace VWolf {
    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
        }

        //VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!"); // ??????
        return 0;
    }

    struct BufferElement
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        size_t Offset;
        bool Normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
        {
        }

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
            case ShaderDataType::Float:   return 1;
            case ShaderDataType::Float2:  return 2;
            case ShaderDataType::Float3:  return 3;
            case ShaderDataType::Float4:  return 4;
            case ShaderDataType::Mat3:    return 3; // 3* float3
            case ShaderDataType::Mat4:    return 4; // 4* float4
            case ShaderDataType::Int:     return 1;
            case ShaderDataType::Int2:    return 2;
            case ShaderDataType::Int3:    return 3;
            case ShaderDataType::Int4:    return 4;
            case ShaderDataType::Bool:    return 1;
            }

            //VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!"); // ??????
            return 0;
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() {}

        BufferLayout(std::initializer_list<BufferElement> elements)
            : m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        uint32_t GetStride() const { return m_Stride; }
        const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    private:
        void CalculateOffsetsAndStride()
        {
            size_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }
    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };


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

        inline std::vector<float> Transform() {
            std::vector<float> values;
            values.push_back(position.x);
            values.push_back(position.y);
            values.push_back(position.z);
            values.push_back(color.r);
            values.push_back(color.g);
            values.push_back(color.b);
            values.push_back(color.a);
            
            return values;
        }
    };

    struct MeshData {
    public:
        inline std::vector<float> GetVertices() {
            std::vector<float> vertices;
            
            for (Vertex vertex: this->vertices) {
                std::vector<float> data = vertex.Transform();
                vertices.insert(vertices.end(), data.begin(), data.end());
            }
            return vertices;
        }
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;

        static BufferLayout Layout;
    };

    inline BufferLayout MeshData::Layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" }
    };
}

