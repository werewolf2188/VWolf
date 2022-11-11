//
//  VertexInfo.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once
#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/Math.h"
#include <map>
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
        case ShaderDataType::None:     return 0;
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
            case ShaderDataType::None:    return 0;
            }

            //VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!"); // ??????
            return 0;
        }
    };

    // Buffer Layout per vertex
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

    // Vertex structure (intercalated)
    struct Vertex {
        Vertex() {
            position = Vector3Float(0, 0, 0);
            color = Vector4Float(0, 0, 0, 1);
            normal = Vector3Float(0, 0, 0);
            tangent = Vector3Float(0, 0, 0);
            texCoord = Vector2Float(0, 0);
        }

        Vertex(float x, float y, float z, float r, float g, float b, float a) {
            position = Vector3Float(x, y, z);
            color = Vector4Float(r, g, b, a);
            normal = Vector3Float(0, 0, 0);
            tangent = Vector3Float(0, 0, 0);
            texCoord = Vector2Float(0, 0);
        }

        Vertex(float x, float y, float z,
               float r, float g, float b, float a,
               float nx, float ny, float nz,
               float tnx, float tny, float tnz,
               float tx, float ty) {
            position = Vector3Float(x, y, z);
            color = Vector4Float(r, g, b, a);
            normal = Vector3Float(nx, ny, nz);
            tangent = Vector3Float(tnx, tny, tnz);
            texCoord = Vector2Float(tx, ty);
        }

        Vertex(Vector3Float pos, Vector4Float col) {
            position = pos;
            color = col;
            normal = Vector3Float(0, 0, 0);
            tangent = Vector3Float(0, 0, 0);
            texCoord = Vector2Float(0, 0);
        }

        Vertex(Vector3Float pos, Vector4Float col, Vector3Float norm, Vector3Float tan, Vector2Float tex) {
            position = pos;
            color = col;
            normal = norm;
            tangent = tan;
            texCoord = tex;
        }

        Vector3Float position;
        Vector4Float color;
        Vector3Float normal;
        Vector3Float tangent;
        Vector2Float texCoord;

        inline std::vector<float> Transform() {
            std::vector<float> values;
            values.push_back(position.x);
            values.push_back(position.y);
            values.push_back(position.z);
            values.push_back(color.r);
            values.push_back(color.g);
            values.push_back(color.b);
            values.push_back(color.a);
            values.push_back(normal.x);
            values.push_back(normal.y);
            values.push_back(normal.z);
            values.push_back(tangent.x);
            values.push_back(tangent.y);
            values.push_back(tangent.z);
            values.push_back(texCoord.x);
            values.push_back(texCoord.y);
            
            return values;
        }
    };

    // Mesh data
    struct MeshData {
    public:
        inline std::vector<float> GetVertices() {
            if (_vertices.size() > 0) {
                return _vertices;
            }

            for (Vertex vertex: this->vertices) {
                std::vector<float> data = vertex.Transform();
                _vertices.insert(_vertices.end(), data.begin(), data.end());
            }
            return _vertices;
        }
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;

        static BufferLayout Layout;
    private:
        std::vector<float> _vertices;
    };

    inline BufferLayout MeshData::Layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
        { ShaderDataType::Float3, "a_Normal" },
        { ShaderDataType::Float3, "a_Tangent" },
        { ShaderDataType::Float2, "a_TexCoord" }
    };

    // Camera Pass
    struct CameraPass {
        /* TODO: I don't know what this value does
             float cbPerObjectPad1;
         */
        MatrixFloat4x4 view;
        MatrixFloat4x4 invView;
        MatrixFloat4x4 proj;
        MatrixFloat4x4 invProj;
        MatrixFloat4x4 viewProjection;
        MatrixFloat4x4 invViewProjection;
        Vector3Float eyePosition;
        Vector2Float renderTargetSize;
        Vector2Float invRenderTargetSize;
        float nearZ;
        float farZ;
        float totalTime;
        float deltaTime;
    };


    // Materials
    class AbstractMaterial {
    public:
        virtual size_t GetSize() = 0;
        virtual std::string GetShader() = 0;
        virtual const char* GetName() = 0;
        virtual void* GetDataPointer() = 0;
    };

    template<typename T>
    class Material: public AbstractMaterial {
    public:
        Material(): shaderName(""), materialName("") {
            pointer = new T();
        }
        Material(const std::string shaderName, const char* materialName): shaderName(shaderName), materialName(materialName) {
            pointer = new T();
        }
        
        virtual size_t GetSize() override {
            return sizeof(T);
        }

        virtual std::string GetShader() override {
            return shaderName;
        }
        
        virtual void* GetDataPointer() override {
            return pointer;
        }
        
        virtual const char* GetName() override {
            return materialName;
        }
        
        T* GetChildObject() {
            return pointer;
        }
    private:
        std::string shaderName;
        const char* materialName;
        std::map<std::string, Color> colors;
        std::map<std::string, Vector3Float> vectors3d;
        std::map<std::string, float> values;
        T* pointer = nullptr;
    };

    // Lights
    struct Light {
    public:
        enum class LightType: unsigned int {
            Unknown = 0,
            Directional = 1,
            Spot = 2,
            Point = 3
        };
        
        Vector4Float color;
        Vector4Float position;
        Vector4Float direction;
        Vector4Float strength;
        float cutOff;
        float exponent;
        LightType type = LightType::Directional;
        

    public:
        static const char* LightName;
    };

    inline const char* Light::LightName = "Light";
}

