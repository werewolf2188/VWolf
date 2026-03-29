//
//  DXILHelpers.cpp
//  VWolf
//
//  Created by Enrique Moises on 4/3/26.
//

#include "vwpch.h"
#include "DXILHelpers.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#else
#include "d3d12shader.h"
#endif

#define DXSC_INITIALIZE \
HRESULT hr;

#define DXSC_EXECUTE(code) \
hr = code;\
if (FAILED(hr)) {\
    VWOLF_CORE_ASSERT(#code);\
}

#define DXSC_EXECUTE_RESULT_AND_CATCH_ERRORS_IF_ANY(variable, function) \
variable->function(&hr); \
if (FAILED(hr)) {\
    SmartPoint<IDxcBlobUtf8> errors;\
    variable->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);\
    if (errors && errors->GetStringPointer()) { \
        VWOLF_CORE_ASSERT(errors->GetStringPointer());\
    } \
}

// https://rtarun9.github.io/blogs/shader_reflection/#reflecting-shader-bound-resources-cbuffers-textures-etc

namespace VWolf {
    namespace DXIL {

    // MARK: Global functions
        const char* ShaderTypeEquivalent(ShaderType type) {
            switch (type) {
            case ShaderType::Vertex: return "vs_6_0";
            case ShaderType::Fragment: return "ps_6_0";
            case ShaderType::Pre_Tesselator: return "hs_6_0";
            case ShaderType::Post_Tesselator: return "ds_6_0";
            case ShaderType::Geometry: return "gs_6_0";
            case ShaderType::Compute: return "cs_6_0";
            }
        }

        std::string ShaderFileName(ShaderSource shader) {
            std::filesystem::path path = shader.shader;
            
            std::string name = path.filename();
            std::string type = ShaderTypeEquivalent(shader.type);
            
            return (name + "." + type + ".cso");
        }

        std::wstring ShaderTypeEquivalentWide(ShaderType type) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring result = converter.from_bytes(ShaderTypeEquivalent(type));
            return result;
        }

        std::wstring ShaderWideFileName(ShaderSource shader) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            
            std::string name = ShaderFileName(shader);
            std::wstring result = converter.from_bytes(name);
            return result;
        }
    // MARK: Attribute
        Attribute::Attribute(_D3D12_SIGNATURE_PARAMETER_DESC* desc, UINT index, UINT offset):
        index(index), offset(offset), elementType(desc->ComponentType)  {
            this->name = desc->SemanticName;
            BYTE oneElement = 1, twoElement = 3, threeElement = 7, fourElement = 15;
            
            if (desc->Mask == oneElement)
            {
                numberOfElements = 1;
                if (desc->ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    this->format = DXGI_FORMAT_R32_UINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    this->format = DXGI_FORMAT_R32_SINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    this->format = DXGI_FORMAT_R32_FLOAT;
            }
            else if (desc->Mask <= twoElement)
            {
                numberOfElements = 2;
                if (desc->ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    this->format = DXGI_FORMAT_R32G32_UINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    this->format = DXGI_FORMAT_R32G32_SINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    this->format = DXGI_FORMAT_R32G32_FLOAT;
            }
            else if (desc->Mask <= threeElement)
            {
                numberOfElements = 3;
                if (desc->ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    this->format = DXGI_FORMAT_R32G32B32_UINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    this->format = DXGI_FORMAT_R32G32B32_SINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    this->format = DXGI_FORMAT_R32G32B32_FLOAT;
            }
            else if (desc->Mask <= fourElement)
            {
                numberOfElements = 4;
                if (desc->ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                    this->format = DXGI_FORMAT_R32G32B32A32_UINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                    this->format = DXGI_FORMAT_R32G32B32A32_SINT;
                else if (desc->ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                    this->format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
        }
    
        Attribute::~Attribute() {
            
        }
    
        size_t Attribute::GetSize() {
            if (elementType == D3D_REGISTER_COMPONENT_UINT32)
                return sizeof(unsigned int) * numberOfElements;
            else if (elementType == D3D_REGISTER_COMPONENT_SINT32)
                return sizeof(int) * numberOfElements;
            else if (elementType == D3D_REGISTER_COMPONENT_FLOAT32)
                return sizeof(float) * numberOfElements;
            return -1;
        }

    // MARK: Constant Buffer variable
    
        ConstantBufferVariable::ConstantBufferVariable(ID3D12ShaderReflectionVariable* vCb, UINT index): index(index) {
            // if NULL, throw exception
            ID3D12ShaderReflectionType* vCbType = vCb->GetType();

            D3D12_SHADER_VARIABLE_DESC vCbDesc{};
            vCb->GetDesc(&vCbDesc);

            D3D12_SHADER_TYPE_DESC vTypeCbDesc{};
            vCbType->GetDesc(&vTypeCbDesc);

            this->name = vCbDesc.Name;
            this->offset = vCbDesc.StartOffset;
            this->size = vCbDesc.Size;

            this->_type = vTypeCbDesc.Type;
            this->_class = vTypeCbDesc.Class;
            this->_columns = vTypeCbDesc.Columns;
        }
    
        ConstantBufferVariable::ConstantBufferVariable(const ConstantBufferVariable& constantBufferVariable) {
            this->name = constantBufferVariable.name;
            this->size = constantBufferVariable.size;
            this->offset = constantBufferVariable.offset;
            this->index = constantBufferVariable.index;

            this->_type = constantBufferVariable._type;
            this->_class = constantBufferVariable._class;
            this->_columns = constantBufferVariable._columns;
        }
    
        ConstantBufferVariable::~ConstantBufferVariable() {}
    
        ConstantBufferVariable& ConstantBufferVariable::operator=(const ConstantBufferVariable& constantBufferVariable) {
            this->name = constantBufferVariable.name;
            this->size = constantBufferVariable.size;
            this->offset = constantBufferVariable.offset;
            this->index = constantBufferVariable.index;

            this->_type = constantBufferVariable._type;
            this->_class = constantBufferVariable._class;
            this->_columns = constantBufferVariable._columns;
            
            return *this;
        }
        
        ShaderDataType ConstantBufferVariable::GetShaderDataType() {
            switch (_type) {
            case D3D_SVT_BOOL:
                return ShaderDataType::Bool;
            case D3D_SVT_INT:
                switch (_class) {
                    case D3D_SVC_SCALAR:
                        return ShaderDataType::Int;
                    case D3D_SVC_VECTOR:
                        switch (_columns) {
                            case 2: return ShaderDataType::Int2;
                            case 3: return ShaderDataType::Int3;
                            case 4: return ShaderDataType::Int4;
                        }
                }
            case D3D_SVT_FLOAT:
                switch (_class) {
                case D3D_SVC_SCALAR:
                    return ShaderDataType::Float;
                case D3D_SVC_VECTOR:
                    switch (_columns) {
                    case 2: return ShaderDataType::Float2;
                    case 3: return ShaderDataType::Float3;
                    case 4: return ShaderDataType::Float4;
                    }
                }

            }

            return ShaderDataType::None;
        }
    
    // MARK: Constant Buffer
    
        ConstantBuffer::ConstantBuffer(_D3D12_SHADER_BUFFER_DESC* desc, UINT bindingIndex):
        name(desc->Name), size(desc->Size), bindingIndex(bindingIndex) {}
    
        ConstantBuffer::ConstantBuffer(const ConstantBuffer& constantBuffer) {
            this->name = constantBuffer.name;
            this->size = constantBuffer.size;
            this->bindingIndex = constantBuffer.bindingIndex;
            
            for(const ConstantBufferVariable& variable: constantBuffer.variables) {
                this->variables.push_back(ConstantBufferVariable(variable));
            }
        }
    
        ConstantBuffer::~ConstantBuffer() {}
    
        ConstantBuffer& ConstantBuffer::operator=(const ConstantBuffer& constantBuffer) {
            this->name = constantBuffer.name;
            this->size = constantBuffer.size;
            this->bindingIndex = constantBuffer.bindingIndex;
            
            for(const ConstantBufferVariable& variable: constantBuffer.variables) {
                this->variables.push_back(ConstantBufferVariable(variable));
            }
            
            return *this;
        }
    
        void ConstantBuffer::ExtractVariables(ID3D12ShaderReflectionConstantBuffer* buffer, _D3D12_SHADER_BUFFER_DESC* bufferDesc) {
            for (uint32_t idy = 0; idy < bufferDesc->Variables; idy++) {
                ID3D12ShaderReflectionVariable* variable = buffer->GetVariableByIndex(idy);
                D3D12_SHADER_VARIABLE_DESC varDesc{};
                variable->GetDesc(&varDesc);
                
                variables.emplace_back(ConstantBufferVariable(variable, idy));
            }
        }

    // MARK: Texture
    
        Texture::Texture(_D3D12_SHADER_INPUT_BIND_DESC* desc):
            name(desc->Name), bindingIndex(desc->BindPoint), returnType(desc->ReturnType), dimension(desc->Dimension) { }
    
        Texture::Texture(const Texture& texture) {
            this->name = texture.name;
            this->bindingIndex = texture.bindingIndex;
            this->returnType = texture.returnType;
            this->dimension = texture.dimension;
        }
    
        Texture::~Texture() { }
    
        Texture& Texture::operator=(const Texture& texture) {
            this->name = texture.name;
            this->bindingIndex = texture.bindingIndex;
            this->returnType = texture.returnType;
            this->dimension = texture.dimension;
            
            return *this;
        }

        UINT Texture::GetTextureType() {
            switch (returnType) {
                case D3D_RETURN_TYPE_FLOAT: {
                    switch (dimension) {
                    case D3D_SRV_DIMENSION_TEXTURE2D: return (int)ShaderSamplerType::Sampler2D;
                    case D3D_SRV_DIMENSION_TEXTURECUBE:
                        return (int)ShaderSamplerType::SamplerCube;
                    }
                }
                case D3D_RETURN_TYPE_SINT: {
                    switch (dimension) {
                    case D3D_SRV_DIMENSION_TEXTURE2D: return (int)ShaderSamplerType::Sampler2D;
                    case D3D_SRV_DIMENSION_TEXTURECUBE:
                        return (int)ShaderSamplerType::SamplerCube;
                    }
                }
            }
            return -1;
        }

        ShaderDataType Texture::GetShaderDataType() {
            switch (returnType) {
            case D3D_RETURN_TYPE_FLOAT: {
                switch (dimension) {
                case D3D_SRV_DIMENSION_TEXTURE1D: return ShaderDataType::Float;
                case D3D_SRV_DIMENSION_TEXTURE2D: return ShaderDataType::Float2;
                case D3D_SRV_DIMENSION_TEXTURE3D: return ShaderDataType::Float3;
                case D3D_SRV_DIMENSION_TEXTURECUBE:
                    return ShaderDataType::Float3;
                }
            }
            case D3D_RETURN_TYPE_SINT: {
                switch (dimension) {
                case D3D_SRV_DIMENSION_TEXTURE1D: return ShaderDataType::Int;
                case D3D_SRV_DIMENSION_TEXTURE2D: return ShaderDataType::Int2;
                case D3D_SRV_DIMENSION_TEXTURE3D: return ShaderDataType::Int3;
                case D3D_SRV_DIMENSION_TEXTURECUBE:
                    return ShaderDataType::Int3;
                }
            }
            case D3D_RETURN_TYPE_UNORM: return ShaderDataType::Bool;
            }
            return ShaderDataType::None;
        }
    
    // MARK: Sampler    
        Sampler::Sampler(_D3D12_SHADER_INPUT_BIND_DESC* desc) :
            name(desc->Name), bindingIndex(desc->BindPoint), numSamples(desc->NumSamples) { }
    
        Sampler::Sampler(const Sampler& sampler) {
            this->name = sampler.name;
            this->bindingIndex = sampler.bindingIndex;
            this->numSamples = sampler.numSamples;
        }
        
        Sampler::~Sampler() {}
    
        Sampler& Sampler::operator=(const Sampler& sampler) {
            this->name = sampler.name;
            this->bindingIndex = sampler.bindingIndex;
            this->numSamples = sampler.numSamples;
            
            return *this;
        }
    
    // MARK: Shader
        Shader::Shader(ShaderSource otherShader): otherShader(otherShader), name(ShaderFileName(otherShader)) {
            CompileHLSLWithDirectXShaderCompiler();
        }
    
        Shader::~Shader() {}
    
        void Shader::CompileHLSLWithDirectXShaderCompiler() {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            uint32_t codePage = DXC_CP_ACP;
                
            // DirectX Shader Compiler execution
            // DirectX Shader Compiler arguments
            DXSC_INITIALIZE
            std::wstring wide = converter.from_bytes(otherShader.shader);
            const wchar_t* wFilename = wide.c_str();
            
            SmartPoint<IDxcUtils> utils;
            SmartPoint<IDxcCompiler3> compiler;
            SmartPoint<IDxcIncludeHandler> includeHandler;
            SmartPoint<IDxcBlobEncoding> sourceBlob;
            
            DXSC_EXECUTE(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)));
            DXSC_EXECUTE(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
            DXSC_EXECUTE(utils->CreateDefaultIncludeHandler(&includeHandler));
            
            DXSC_EXECUTE(utils->LoadFile(wFilename, &codePage, &sourceBlob));
            
            DxcBuffer src = {
                sourceBlob->GetBufferPointer(),
                sourceBlob->GetBufferSize(),
                codePage
            };
            
            wide = converter.from_bytes(otherShader.mainFunction);
            std::wstring outputName = ShaderWideFileName(otherShader);
            std::wstring type = ShaderTypeEquivalentWide(otherShader.type);
            
            LPCWSTR arguments[] = {
                L"-E", wide.c_str(),                                    // Entry point name: "Main"
                L"-T", type.c_str(),                                    // Target profile: Vertex Shader Model 6.0
                DXC_ARG_DEBUG,                                          // Enable debug info (PDB)
                DXC_ARG_SKIP_OPTIMIZATIONS,
                DXC_ARG_ALL_RESOURCES_BOUND,
                L"-Qembed_debug",
                L"-Fo", outputName.c_str()                              // Output object file name
            };
            
            UINT32 argCount = _countof(arguments);
            
            SmartPoint<IDxcResult> result;
            DXSC_EXECUTE(compiler->Compile(
                                           &src,                   // Source buffer
                                           arguments,              // Arguments
                                           argCount,               // Argument count
                                           DEREFERENCE(includeHandler),   // Include handler
                                           IID_PPV_ARGS(&result)   // Operation result
                                           ));
            
            DXSC_EXECUTE_RESULT_AND_CATCH_ERRORS_IF_ANY(result, GetStatus);
            
            DXSC_EXECUTE(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr));
            
            if (pShader != nullptr) {
                // Save the shader binary to a file (e.g., shader.cso) or use it directly in your application
                // ... (file writing code omitted for brevity)
                std::wcout << L"Compilation successful. Generated shader bytecode size: " << pShader->GetBufferSize() << L" bytes." << std::endl;
            }
            
            SmartPoint<IDxcBlob> pReflectionData;
            DXSC_EXECUTE(result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr));
            
            const DxcBuffer reflectionBuffer
            {
                .Ptr = pReflectionData->GetBufferPointer(),
                .Size = pReflectionData->GetBufferSize(),
                .Encoding = 0,
            };
            
            SmartPoint<ID3D12ShaderReflection> shaderReflection;
            DXSC_EXECUTE(utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&shaderReflection)));
            DXILReflection(shaderReflection);
        }
    
        void Shader::DXILReflection(SmartPoint<ID3D12ShaderReflection> shaderReflection) {
            DXSC_INITIALIZE
            D3D12_SHADER_DESC shaderDesc{};
            DXSC_EXECUTE(shaderReflection->GetDesc(&shaderDesc));
            
            if (otherShader.type == ShaderType::Vertex)
                GetStageInAttributes(shaderReflection, &shaderDesc);
            GetInputBinds(shaderReflection, &shaderDesc);
        }
    
        void Shader::GetInputBinds(SmartPoint<ID3D12ShaderReflection> shaderReflection, _D3D12_SHADER_DESC* shaderDesc) {
            DXSC_INITIALIZE
            
            for (uint32_t idx = 0; idx < shaderDesc->BoundResources; idx++) {
                D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc{};
                DXSC_EXECUTE(shaderReflection->GetResourceBindingDesc(idx, &shaderInputBindDesc));
                
                if (shaderInputBindDesc.Type == D3D_SIT_CBUFFER) {
                    
                    ID3D12ShaderReflectionConstantBuffer* shaderReflectionConstantBuffer = shaderReflection->GetConstantBufferByIndex(idx);
                    D3D12_SHADER_BUFFER_DESC constantBufferDesc{};
                    shaderReflectionConstantBuffer->GetDesc(&constantBufferDesc);
                    ConstantBuffer cb(&constantBufferDesc, shaderInputBindDesc.BindPoint);
                    cb.ExtractVariables(shaderReflectionConstantBuffer, &constantBufferDesc);
                    constantBuffers.emplace_back(cb);
                } else if (shaderInputBindDesc.Type == D3D_SIT_TEXTURE) {
                    textures.emplace_back(Texture(&shaderInputBindDesc));
                } else if (shaderInputBindDesc.Type == D3D_SIT_SAMPLER) {
                    samplers.emplace_back(Sampler(&shaderInputBindDesc));
                }
            }
        }
    
        void Shader::GetStageInAttributes(SmartPoint<ID3D12ShaderReflection> shaderReflection, _D3D12_SHADER_DESC* shaderDesc) {
            UINT offset = 0;
            for (uint32_t idx = 0; idx < shaderDesc->InputParameters; idx++) {
                D3D12_SIGNATURE_PARAMETER_DESC signatureParameterDesc{};
                shaderReflection->GetInputParameterDesc(idx, &signatureParameterDesc);
                Attribute attr(&signatureParameterDesc, idx, offset);
                offset += attr.GetSize();
                stageInAttributes.emplace_back(attr);
            }
        }
    }
}
