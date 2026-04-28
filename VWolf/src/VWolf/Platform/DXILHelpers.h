//
//  DXILHelpers.hpp
//  VWolf
//
//  Created by Enrique Moises on 4/3/26.
//

#pragma once

#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Core/Render/Shader.h"

#include "dxcapi.h"

#ifdef VWOLF_PLATFORM_WINDOWS
template<typename T>
using SmartPoint = Microsoft::WRL::ComPtr<T>;
#define DEREFERENCE(SP) SP.Get()
#else
template<typename T>
using SmartPoint = CComPtr<T>;
#define DEREFERENCE(SP) &*SP
#endif

struct ID3D12ShaderReflection;
struct ID3D12ShaderReflectionConstantBuffer;
struct ID3D12ShaderReflectionVariable;
struct _D3D12_SIGNATURE_PARAMETER_DESC;
struct _D3D12_SHADER_DESC;
struct _D3D12_SHADER_BUFFER_DESC;
struct _D3D12_SHADER_INPUT_BIND_DESC;

namespace VWolf {
    namespace DXIL {
    
        enum DXGI_FORMAT {
          DXGI_FORMAT_UNKNOWN = 0,
          DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
          DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
          DXGI_FORMAT_R32G32B32A32_UINT = 3,
          DXGI_FORMAT_R32G32B32A32_SINT = 4,
          DXGI_FORMAT_R32G32B32_TYPELESS = 5,
          DXGI_FORMAT_R32G32B32_FLOAT = 6,
          DXGI_FORMAT_R32G32B32_UINT = 7,
          DXGI_FORMAT_R32G32B32_SINT = 8,
          DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
          DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
          DXGI_FORMAT_R16G16B16A16_UNORM = 11,
          DXGI_FORMAT_R16G16B16A16_UINT = 12,
          DXGI_FORMAT_R16G16B16A16_SNORM = 13,
          DXGI_FORMAT_R16G16B16A16_SINT = 14,
          DXGI_FORMAT_R32G32_TYPELESS = 15,
          DXGI_FORMAT_R32G32_FLOAT = 16,
          DXGI_FORMAT_R32G32_UINT = 17,
          DXGI_FORMAT_R32G32_SINT = 18,
          DXGI_FORMAT_R32G8X24_TYPELESS = 19,
          DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
          DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
          DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
          DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
          DXGI_FORMAT_R10G10B10A2_UNORM = 24,
          DXGI_FORMAT_R10G10B10A2_UINT = 25,
          DXGI_FORMAT_R11G11B10_FLOAT = 26,
          DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
          DXGI_FORMAT_R8G8B8A8_UNORM = 28,
          DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
          DXGI_FORMAT_R8G8B8A8_UINT = 30,
          DXGI_FORMAT_R8G8B8A8_SNORM = 31,
          DXGI_FORMAT_R8G8B8A8_SINT = 32,
          DXGI_FORMAT_R16G16_TYPELESS = 33,
          DXGI_FORMAT_R16G16_FLOAT = 34,
          DXGI_FORMAT_R16G16_UNORM = 35,
          DXGI_FORMAT_R16G16_UINT = 36,
          DXGI_FORMAT_R16G16_SNORM = 37,
          DXGI_FORMAT_R16G16_SINT = 38,
          DXGI_FORMAT_R32_TYPELESS = 39,
          DXGI_FORMAT_D32_FLOAT = 40,
          DXGI_FORMAT_R32_FLOAT = 41,
          DXGI_FORMAT_R32_UINT = 42,
          DXGI_FORMAT_R32_SINT = 43,
          DXGI_FORMAT_R24G8_TYPELESS = 44,
          DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
          DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
          DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
          DXGI_FORMAT_R8G8_TYPELESS = 48,
          DXGI_FORMAT_R8G8_UNORM = 49,
          DXGI_FORMAT_R8G8_UINT = 50,
          DXGI_FORMAT_R8G8_SNORM = 51,
          DXGI_FORMAT_R8G8_SINT = 52,
          DXGI_FORMAT_R16_TYPELESS = 53,
          DXGI_FORMAT_R16_FLOAT = 54,
          DXGI_FORMAT_D16_UNORM = 55,
          DXGI_FORMAT_R16_UNORM = 56,
          DXGI_FORMAT_R16_UINT = 57,
          DXGI_FORMAT_R16_SNORM = 58,
          DXGI_FORMAT_R16_SINT = 59,
          DXGI_FORMAT_R8_TYPELESS = 60,
          DXGI_FORMAT_R8_UNORM = 61,
          DXGI_FORMAT_R8_UINT = 62,
          DXGI_FORMAT_R8_SNORM = 63,
          DXGI_FORMAT_R8_SINT = 64,
          DXGI_FORMAT_A8_UNORM = 65,
          DXGI_FORMAT_R1_UNORM = 66,
          DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
          DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
          DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
          DXGI_FORMAT_BC1_TYPELESS = 70,
          DXGI_FORMAT_BC1_UNORM = 71,
          DXGI_FORMAT_BC1_UNORM_SRGB = 72,
          DXGI_FORMAT_BC2_TYPELESS = 73,
          DXGI_FORMAT_BC2_UNORM = 74,
          DXGI_FORMAT_BC2_UNORM_SRGB = 75,
          DXGI_FORMAT_BC3_TYPELESS = 76,
          DXGI_FORMAT_BC3_UNORM = 77,
          DXGI_FORMAT_BC3_UNORM_SRGB = 78,
          DXGI_FORMAT_BC4_TYPELESS = 79,
          DXGI_FORMAT_BC4_UNORM = 80,
          DXGI_FORMAT_BC4_SNORM = 81,
          DXGI_FORMAT_BC5_TYPELESS = 82,
          DXGI_FORMAT_BC5_UNORM = 83,
          DXGI_FORMAT_BC5_SNORM = 84,
          DXGI_FORMAT_B5G6R5_UNORM = 85,
          DXGI_FORMAT_B5G5R5A1_UNORM = 86,
          DXGI_FORMAT_B8G8R8A8_UNORM = 87,
          DXGI_FORMAT_B8G8R8X8_UNORM = 88,
          DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
          DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
          DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
          DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
          DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
          DXGI_FORMAT_BC6H_TYPELESS = 94,
          DXGI_FORMAT_BC6H_UF16 = 95,
          DXGI_FORMAT_BC6H_SF16 = 96,
          DXGI_FORMAT_BC7_TYPELESS = 97,
          DXGI_FORMAT_BC7_UNORM = 98,
          DXGI_FORMAT_BC7_UNORM_SRGB = 99,
          DXGI_FORMAT_AYUV = 100,
          DXGI_FORMAT_Y410 = 101,
          DXGI_FORMAT_Y416 = 102,
          DXGI_FORMAT_NV12 = 103,
          DXGI_FORMAT_P010 = 104,
          DXGI_FORMAT_P016 = 105,
          DXGI_FORMAT_420_OPAQUE = 106,
          DXGI_FORMAT_YUY2 = 107,
          DXGI_FORMAT_Y210 = 108,
          DXGI_FORMAT_Y216 = 109,
          DXGI_FORMAT_NV11 = 110,
          DXGI_FORMAT_AI44 = 111,
          DXGI_FORMAT_IA44 = 112,
          DXGI_FORMAT_P8 = 113,
          DXGI_FORMAT_A8P8 = 114,
          DXGI_FORMAT_B4G4R4A4_UNORM = 115,
          DXGI_FORMAT_P208 = 130,
          DXGI_FORMAT_V208 = 131,
          DXGI_FORMAT_V408 = 132,
          DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
          DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
          DXGI_FORMAT_FORCE_UINT = 0xffffffff
        };
    
        struct Attribute {
        public:
            Attribute() = default;
            Attribute(_D3D12_SIGNATURE_PARAMETER_DESC* desc, UINT index, UINT offset);
            
            ~Attribute();
        public:
            std::string& GetName() { return name; }
            UINT GetOffset() { return offset; }
            UINT GetIndex() { return index; }
            DXGI_FORMAT GetFormat() { return format; }
			UINT GetNumberOfElements() { return numberOfElements; }
			BYTE GetElementType() { return elementType; }
        public:
            size_t GetSize();
        private:
            std::string name;
            UINT index;
            DXGI_FORMAT format;
            UINT offset;
            
            UINT numberOfElements;
            BYTE elementType;
        };
    
        class ConstantBufferVariable {
        public:
            ConstantBufferVariable() = default;
            ConstantBufferVariable(ID3D12ShaderReflectionVariable* vCb, UINT index);
            ConstantBufferVariable(const ConstantBufferVariable& constantBufferVariable);
            ~ConstantBufferVariable();
        public:
            ConstantBufferVariable& operator=(const ConstantBufferVariable& other);
        public:
            std::string& GetName() { return name; }
            size_t GetSize() { return size; }
            UINT GetOffset() { return offset; }
            UINT GetIndex() { return index; }
        public:
			UINT GetType() { return _type; }
			UINT GetClass() { return _class; }
			UINT GetColumns() { return _columns; }
        public:
            ShaderDataType GetShaderDataType();
        private:
            std::string name;
            size_t size;
            UINT offset;
            UINT index;

            UINT _type;
            UINT _class;
            UINT _columns;
        };
    
        class ConstantBuffer {
        public:
            ConstantBuffer() = default;
            ConstantBuffer(_D3D12_SHADER_BUFFER_DESC* desc, UINT bindingIndex);
            ConstantBuffer(const ConstantBuffer& constantBuffer);
            ~ConstantBuffer();
        public:
            ConstantBuffer& operator=(const ConstantBuffer& other);
        public:
            void ExtractVariables(ID3D12ShaderReflectionConstantBuffer* buffer, _D3D12_SHADER_BUFFER_DESC* bufferDesc);
        public:
            std::string& GetName() { return name; }
            size_t GetSize() { return size; }
            UINT GetBindingIndex() { return bindingIndex; }
            std::vector<ConstantBufferVariable>& GetVariables() { return variables; }
        private:
            std::string name;
            size_t size;
            UINT bindingIndex;
            
            std::vector<ConstantBufferVariable> variables;
        };
    
        class Texture {
        public:
            Texture() = default;
            Texture(const Texture& texture);
            Texture(_D3D12_SHADER_INPUT_BIND_DESC* desc);
            ~Texture();
        public:
            Texture& operator=(const Texture& other);
        public:
            std::string GetName() { return name; }
            UINT GetBindingIndex() { return bindingIndex; }
            UINT GetReturnType() { return returnType; }
            UINT GetDimension() { return dimension; }
        public:
            UINT GetTextureType();
            ShaderDataType GetShaderDataType();
        private:
            std::string name;
            UINT bindingIndex = 0;
            UINT returnType;
            UINT dimension;
        };
    
        class Sampler {
        public:
            Sampler() = default;
            Sampler(const Sampler& sampler);
            Sampler(_D3D12_SHADER_INPUT_BIND_DESC* desc);
            ~Sampler();
        public:
            Sampler& operator=(const Sampler& other);
        public:
            std::string& GetName() { return name; }
            UINT GetBindingIndex() { return bindingIndex; }
            UINT GetNumSamples() { return numSamples; }
        private:
            std::string name;
            UINT bindingIndex = 0;
            UINT numSamples = 0;
        };
    
        struct Shader {
        public:
            enum class ArgumentType {
                DirectX,
                Metal,
                OpenGL
            };
        public:
            Shader();
            Shader(std::string name, VWolf::Stage& stageShader, std::string code, ArgumentType argumentType);
            ~Shader();
        public:
            ShaderType GetType() { return type; }
            ShaderSourceType GetSourceType() { return sourceType; }
            std::string& GetShaderSource() { return shader; }
            const char* GetMainFunction() { return mainFunction.c_str(); }
            SmartPoint<IDxcBlob>& GetShader() { return pShader; }
            std::string& GetName() { return name; }
            std::vector<Attribute>& GetStageInAttributes() { return stageInAttributes; }
            std::vector<ConstantBuffer>& GetConstantBuffers() { return constantBuffers; }
            std::vector<Texture>& GetTextures() { return textures; }
            std::vector<Sampler>& GetSamplers() { return samplers; }
        private:
            void CompileHLSLWithDirectXShaderCompiler(ArgumentType argumentType);
            void DXILReflection(SmartPoint<ID3D12ShaderReflection> shaderReflection);
            void GetStageInAttributes(SmartPoint<ID3D12ShaderReflection> shaderReflection, _D3D12_SHADER_DESC* shaderDesc);
            void GetInputBinds(SmartPoint<ID3D12ShaderReflection> shaderReflection, _D3D12_SHADER_DESC* shaderDesc);
        private:
            ShaderType type;
            ShaderSourceType sourceType;
            std::string shader;
            std::string mainFunction;
            
            SmartPoint<IDxcBlob> pShader;
            std::string name;
            
            std::vector<Attribute> stageInAttributes;
            std::vector<ConstantBuffer> constantBuffers;
            std::vector<Texture> textures;
            std::vector<Sampler> samplers;
        };
    }
}
