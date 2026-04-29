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

#ifdef VWOLF_PLATFORM_WINDOWS
#define GetPointer(SP) SP.Get()
#elif defined(VWOLF_PLATFORM_MACOS)
#define GetPointer(SP) SP.p
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

        std::string ShaderFileName(std::string shader, ShaderType _type) {
            std::filesystem::path path = shader;
            
            std::string name = path.filename().string();
            std::string type = ShaderTypeEquivalent(_type);
            
            return (name + "." + type + ".cso");
        }
    
        std::string ShaderName(std::string name, ShaderType _type) {
            std::string type = ShaderTypeEquivalent(_type);
            
            return (name + "." + type + ".cso");
        }

        std::wstring ShaderTypeEquivalentWide(ShaderType type) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring result = converter.from_bytes(ShaderTypeEquivalent(type));
            return result;
        }

        std::wstring ShaderWideFileName(std::string shader, ShaderType _type) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            
            std::string name = ShaderFileName(shader, _type);
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
    
    // MARK: Include handler
    class VWolfDxcIncludeHandler : public IDxcIncludeHandler {
    public:
        VWolfDxcIncludeHandler(IDxcUtils* pUtils) : m_pUtils(pUtils), m_refCount(1) {}
        /// \brief Load a source file to be included by the compiler.
        ///
        /// \param pFilename Candidate filename.
        ///
        /// \param ppIncludeSource Resultant source object for included file, nullptr
        /// if not found.
        HRESULT STDMETHODCALLTYPE
        LoadSource(_In_z_ LPCWSTR pFilename,
                   _COM_Outptr_result_maybenull_ IDxcBlob **ppIncludeSource) override {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

            // TODO: Move this to add it from outside.
            std::filesystem::path directory = "shaders/include/";
            std::filesystem::path filename = converter.to_bytes(pFilename);
            std::filesystem::path path = directory / filename.filename();
            
//            std::cout << "Including: " << path << std::endl;
            HRESULT code;
            if (std::filesystem::exists(path)) {
                std::wstring widePath = converter.from_bytes(path.string());
                const wchar_t* wPath = widePath.c_str();
                uint32_t codePage = DXC_CP_UTF8;
                code = m_pUtils->LoadFile(wPath, &codePage, (IDxcBlobEncoding**)ppIncludeSource);
            } else {
                code = m_pUtils->
                CreateBlobFromPinned(emptyString.c_str(), (UINT32)emptyString.size(), CP_UTF8, (IDxcBlobEncoding**)ppIncludeSource);
            }
            
            return code;
        }
        
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override {
            if (riid == __uuidof(IDxcIncludeHandler) || riid == __uuidof(IUnknown)) {
                *ppvObject = this;
                AddRef();
                return S_OK;
            }
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }
        
        ULONG STDMETHODCALLTYPE AddRef() override {
            return ++m_refCount;
        }
        
        ULONG STDMETHODCALLTYPE Release() override {
            ULONG newCount = --m_refCount;
            if (newCount == 0) delete this;
            return newCount;
        }
    private:
        std::atomic<long> m_refCount;
        IDxcUtils* m_pUtils;
        std::string emptyString = "";
    };
    
    // MARK: Shader
        Shader::Shader() {}
    
        Shader::Shader(std::string name, VWolf::Stage& stageShader, std::string code, ArgumentType argumentType):
        type(stageShader.GetStageType()), sourceType(ShaderSourceType::Text), shader(code), mainFunction(stageShader.GetFunctionName()),
            name(ShaderName(name, stageShader.GetStageType())) {
            CompileHLSLWithDirectXShaderCompiler(argumentType);
        }
    
        Shader::~Shader() {}
    
        void Shader::CompileHLSLWithDirectXShaderCompiler(ArgumentType argumentType) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            uint32_t codePage = DXC_CP_ACP;
                
            // DirectX Shader Compiler execution
            // DirectX Shader Compiler arguments
            DXSC_INITIALIZE
            std::wstring wide = converter.from_bytes(shader);
            const wchar_t* wShader = wide.c_str();
            
            SmartPoint<IDxcUtils> utils;
            SmartPoint<IDxcCompiler3> compiler;
            SmartPoint<IDxcBlobEncoding> sourceBlob;
            
            DXSC_EXECUTE(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)));
            DXSC_EXECUTE(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
            VWolfDxcIncludeHandler handler(GetPointer(utils));
            SmartPoint<IDxcIncludeHandler> includeHandler(&handler);
//            SmartPoint<IDxcIncludeHandler> includeHandler;
//            DXSC_EXECUTE(utils->CreateDefaultIncludeHandler(&includeHandler));
            
            DxcBuffer src;
            
            switch (sourceType) {
                case VWolf::ShaderSourceType::File:
                    {
                        DXSC_EXECUTE(utils->LoadFile(wShader, &codePage, &sourceBlob));
                        src = {
                            sourceBlob->GetBufferPointer(),
                            sourceBlob->GetBufferSize(),
                            codePage
                        };
                    }
                    break;
                case VWolf::ShaderSourceType::Text:
                    {
                        src.Ptr = shader.c_str();
                        src.Size = shader.size();
                        src.Encoding = DXC_CP_UTF8;
                    }
                    break;
                default: break;
            }
            
            wide = converter.from_bytes(mainFunction);
            std::wstring outputName = ShaderWideFileName(shader, this->type);
            std::wstring type = ShaderTypeEquivalentWide(this->type);
            UINT32 argCount = 0;
            LPCWSTR* arguments;
            switch (argumentType) {
                case Shader::ArgumentType::DirectX:
                case Shader::ArgumentType::Metal:
                    arguments = new LPCWSTR[] {
                        L"-E", wide.c_str(),                                    // Entry point name: "Main"
                        L"-T", type.c_str(),                                    // Target profile: Vertex Shader Model 6.0
                        DXC_ARG_DEBUG,                                          // Enable debug info (PDB)
                        DXC_ARG_SKIP_OPTIMIZATIONS,
                        DXC_ARG_ALL_RESOURCES_BOUND,
                        L"-Qembed_debug",
                        L"-Fo", outputName.c_str()                              // Output object file name
                    };
                    argCount = 10;
                    break;
                case Shader::ArgumentType::OpenGL:
                    arguments = new LPCWSTR[] {
                        L"-E", wide.c_str(),                                    // Entry point name: "Main"
                        L"-T", type.c_str(),                                    // Target profile: Vertex Shader Model 6.0
                        DXC_ARG_DEBUG,                                          // Enable debug info (PDB)
                        DXC_ARG_SKIP_OPTIMIZATIONS,
                        DXC_ARG_ALL_RESOURCES_BOUND,
                        L"-Qembed_debug",
                        L"-spirv",
                        L"-D OPENGL",
                        L"-Fo", outputName.c_str()                              // Output object file name
                    };
                    argCount = 12;
                    break;
            }
            
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
            
            VWOLF_CORE_ASSERT(pShader != nullptr, "Shader was not able to compile");
            
            if (argumentType == Shader::ArgumentType::OpenGL) return;
            
            SmartPoint<IDxcBlob> pReflectionData;
            DXSC_EXECUTE(result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr));
            
            DxcBuffer reflectionBuffer{};
            
            reflectionBuffer.Ptr = pReflectionData->GetBufferPointer();
            reflectionBuffer.Size = pReflectionData->GetBufferSize();
            reflectionBuffer.Encoding = 0;
            
            SmartPoint<ID3D12ShaderReflection> shaderReflection;
            DXSC_EXECUTE(utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&shaderReflection)));
            DXILReflection(shaderReflection);
        }
    
        void Shader::DXILReflection(SmartPoint<ID3D12ShaderReflection> shaderReflection) {
            DXSC_INITIALIZE
            D3D12_SHADER_DESC shaderDesc{};
            DXSC_EXECUTE(shaderReflection->GetDesc(&shaderDesc));
            
            if (type == ShaderType::Vertex)
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
