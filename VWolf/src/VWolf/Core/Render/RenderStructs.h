//
//  VertexInfo.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once
#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"
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
            position = Vector3(0, 0, 0);
            color = Vector4(0, 0, 0, 1);
            normal = Vector3(0, 0, 0);
            tangent = Vector3(0, 0, 0);
            texCoord = Vector2(0, 0);
        }

        Vertex(float x, float y, float z, float r, float g, float b, float a) {
            position = Vector3(x, y, z);
            color = Vector4(r, g, b, a);
            normal = Vector3(0, 0, 0);
            tangent = Vector3(0, 0, 0);
            texCoord = Vector2(0, 0);
        }

        Vertex(float x, float y, float z,
               float r, float g, float b, float a,
               float nx, float ny, float nz,
               float tnx, float tny, float tnz,
               float tx, float ty) {
            position = Vector3(x, y, z);
            color = Vector4(r, g, b, a);
            normal = Vector3(nx, ny, nz);
            tangent = Vector3(tnx, tny, tnz);
            texCoord = Vector2(tx, ty);
        }

        Vertex(Vector3 pos, Vector4 col) {
            position = pos;
            color = col;
            normal = Vector3(0, 0, 0);
            tangent = Vector3(0, 0, 0);
            texCoord = Vector2(0, 0);
        }

        Vertex(Vector3 pos, Vector4 col, Vector3 norm, Vector3 tan, Vector2 tex) {
            position = pos;
            color = col;
            normal = norm;
            tangent = tan;
            texCoord = tex;
        }

        Vector3 position;
        Vector4 color;
        Vector3 normal;
        Vector3 tangent;
        Vector2 texCoord;

        inline std::vector<float> Transform() {
            std::vector<float> values;
            values.push_back(position.GetX());
            values.push_back(position.GetY());
            values.push_back(position.GetZ());
            values.push_back(color.GetX());
            values.push_back(color.GetY());
            values.push_back(color.GetZ());
            values.push_back(color.GetW());
            values.push_back(normal.GetX());
            values.push_back(normal.GetY());
            values.push_back(normal.GetZ());
            values.push_back(tangent.GetX());
            values.push_back(tangent.GetY());
            values.push_back(tangent.GetZ());
            values.push_back(texCoord.GetX());
            values.push_back(texCoord.GetY());
            
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
    
        inline std::string GetName() { return name; }
        inline void SetName(std::string name) { this->name = name; }
    private:
        std::vector<float> _vertices;
        std::string name;
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
        Matrix4x4 view;
        Matrix4x4 invView;
        Matrix4x4 proj;
        Matrix4x4 invProj;
        Matrix4x4 viewProjection;
        Matrix4x4 invViewProjection;
        Vector3 eyePosition;
        float padding;
        Vector2 renderTargetSize;
        Vector2 invRenderTargetSize;
        float nearZ;
        float farZ;
        float totalTime;
        float deltaTime;
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
        
        Vector4 color;
        Vector4 position;
        Vector4 direction;
        Vector4 strength;
        float cutOff;
        float exponent;
        LightType type = LightType::Directional;
        // TODO: This padding is only for OPENGL, but it could be useful for DirectX
        // TODO: In case of not being useful, pass it to the specific render.
        float padding; // Needed for OPENGL

    public:
        Matrix4x4 GetLightSpaceMatrix() {
            Vector3 rotation(direction.GetX(), direction.GetY(), direction.GetZ());
            Vector3 eye(position.GetX(), position.GetY(), position.GetZ());
            Quaternion q = static_cast<Quaternion>(rotation.Orientate());
            Vector3 up(q.GetOrientation({ 0.0f, 1.0f, 0.0f }));
            Vector3 center(q.GetOrientation({ 0.0f, 0.0f, -1.0f }));
            Matrix4x4 lightView = Matrix4x4::LookAt(eye, center, up);
            float near_plane = 1.0f, far_plane = 100.0f;
            Matrix4x4 lightProjection = Matrix4x4::Ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);

            /*
            float near_plane = 1, far_plane = 25.0f;
            MatrixFloat4x4 lightProjection = ortho(-30.0f, 30.0f, 30.0f, -30.0f, near_plane, far_plane);
            */
    
            return lightProjection * lightView;
        }
    public:
        static const char* LightName;
        static const char* LightSpaceName;
        static const int LightsMax;
    };

    inline const char* Light::LightName = "Light";
    inline const char* Light::LightSpaceName = "LightSpace";
    inline const int Light::LightsMax = 8;    
}

