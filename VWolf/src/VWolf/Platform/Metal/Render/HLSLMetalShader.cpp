//
//  HLSLToMetal.cpp
//  VWolf
//
//  Created by Enrique Moises on 3/28/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "HLSLMetalShader.h"

#include "VWolf/Core/Utils/File.h"
#include "VWolf/Platform/Metal/MetalDriver.h"

#include "VWolf/Platform/DXILHelpers.h"

#define IRREFLECTINITIALIZE(type, info)\
type info

#define IRREFLECT(function, reflection, info) \
function(reflection, IRReflectionVersion_1_0, &info)

#define RELEASEMAP(map) \
for(auto kv: map) \
kv.second->release();

// bool IRShaderReflectionCopyVertexInfo(const IRShaderReflection* reflection, IRReflectionVersion version, IRVersionedVSInfo* vsinfo);
#define IR_RUNTIME_METALCPP       // enable metal-cpp compatibility mode
#define IR_PRIVATE_IMPLEMENTATION
#include <metal_irconverter_runtime/metal_irconverter_runtime.h>
 /*
  
  // Google search:
  
  https://www.google.com/search?q=IDxcContainerReflection+get+reflection+of+variables&client=safari&hs=GYuU&sca_esv=6b7c40959298e02e&rls=en&sxsrf=ANbL-n6mSLMFvC4M6A54VIIwNaCfD7hgSQ%3A1774325463490&ei=1w7CaafMHZiFqtsP0Je46QI&biw=1728&bih=933&ved=0ahUKEwjnk9631beTAxWYgmoFHdALLi0Q4dUDCBE&uact=5&oq=IDxcContainerReflection+get+reflection+of+variables&gs_lp=Egxnd3Mtd2l6LXNlcnAiM0lEeGNDb250YWluZXJSZWZsZWN0aW9uIGdldCByZWZsZWN0aW9uIG9mIHZhcmlhYmxlczIHECEYoAEYCjIHECEYoAEYCjIHECEYoAEYCjIHECEYoAEYCkiyOFCjC1jWN3ABeACQAQCYAYkBoAG6FKoBBTE2LjEwuAEDyAEA-AEBmAIboALLFcICCBAAGLADGO8FwgIEECMYJ8ICCBAAGIAEGKIEwgIIEAAYogQYiQXCAgUQABjvBcICBRAhGKsCmAMAiAYBkAYEkgcFMTEuMTagB-iUAbIHBTEwLjE2uAfJFcIHBjAuMjIuNcgHOIAIAA&sclient=gws-wiz-serp
  
  Metal converter
  
  https://developer.apple.com/metal/shader-converter/#complete
  
  Multiline inside YAML
  https://stackoverflow.com/questions/3790454/how-do-i-break-a-string-in-yaml-over-multiple-lines
  
  https://www.google.com/search?client=safari&rls=en&q=xcodebuild+command+build&ie=UTF-8&oe=UTF-8
*/

namespace VWolf {

    ProgramType getShaderExtension(std::initializer_list<ShaderSource> otherShaders) {
        const std::filesystem::path path = otherShaders.begin()->shader;
        if (path.extension() == ".metal") { // OLD SHADER FILE
            return ProgramType::MSL;
        } else if (path.extension() == ".hlsl") { // NEW SHADER FILE
            return ProgramType::HLSL;
        }
        return ProgramType::UNKNOWN;
    }

    uint64_t GetSizeFrom(MTL::VertexFormat format) {
        
        switch (format) {
            case MTL::VertexFormat::VertexFormatFloat: return sizeof(float);
            case MTL::VertexFormat::VertexFormatFloat2: return sizeof(float) * 2;
            case MTL::VertexFormat::VertexFormatFloat3: return sizeof(float) * 3;
            case MTL::VertexFormat::VertexFormatFloat4: return sizeof(float) * 4;
            default: break;
        }
        return -1;
    }

    IRShaderStage ShaderTypeEquivalentIR(ShaderType type) {
        switch (type) {
        case ShaderType::Vertex: return IRShaderStageVertex;
        case ShaderType::Fragment: return IRShaderStageFragment;
        case ShaderType::Pre_Tesselator: return IRShaderStageHull;
        case ShaderType::Post_Tesselator: return IRShaderStageDomain;
        case ShaderType::Geometry: return IRShaderStageGeometry;
        case ShaderType::Compute: return IRShaderStageCompute;
        }
    }

    void process_data(unsigned char* data_buffer, size_t length) {
        // Function that expects a char*
        for (size_t i = 0; i < length; ++i) {
            std::cout << data_buffer[i];
        }
        std::cout << std::endl;
    }

    struct StageInAttribute {
    public:
        StageInAttribute(boost::json::value value): value(value) { }
    public:
        int64_t GetIndex() { return value.at("index").as_int64(); }
        MTL::VertexFormat GetVertexFormat() {
            std::string elementType = value.at("elementType").as_string().c_str();
            int64_t columnCount = value.at("columnCount").as_int64();
            if (elementType == "Float") {
                switch (columnCount) {
                    case 1:
                        return MTL::VertexFormat::VertexFormatFloat;
                    case 2:
                        return MTL::VertexFormat::VertexFormatFloat2;
                    case 3:
                        return MTL::VertexFormat::VertexFormatFloat3;
                    case 4:
                        return MTL::VertexFormat::VertexFormatFloat4;
                    default: break;
                }
            }
            return MTL::VertexFormatInvalid;
        }
        std::string GetName() { return value.at("name").as_string().c_str(); }
        uint64_t GetSize() { return GetSizeFrom(GetVertexFormat()); }
    private:
        boost::json::value value;
    };

    class Argument {
    public:
        Argument(): value(nullptr), shaderType((ShaderType)0) {}
        Argument(boost::json::value value, ShaderType shaderType): value(value), shaderType(shaderType) {
        }
        Argument(const Argument& argument): value(argument.value), shaderType(argument.shaderType) {
        }
        Argument(Argument&& argument): value(argument.value), shaderType(argument.shaderType) {
            argument.value = nullptr;
            argument.shaderType = (ShaderType)0;
        }
    public:
        Argument& operator=(const Argument& argument) {
            this->value = argument.value;
            this->shaderType = argument.shaderType;
            return *this;
        }
    public:
        uint64_t GetIndex() { return value.at("Slot").as_uint64(); }
        uint64_t GetSpace() { return value.at("Space").as_uint64(); }
        std::string GetName() { return value.at("Name").as_string().c_str();}
        uint64_t GetSize() { return value.at("Size").as_uint64(); }
        int64_t GetOffset() { return value.at("EltOffset").as_int64(); }
        IRResourceType GetType() {
            std::string type = value.at("Type").as_string().c_str();
            if (type == "CBV")
                return IRResourceTypeCBV;
            else if (type == "SRV")
                return IRResourceTypeSRV;
            else if (type == "UAV")
                return IRResourceTypeUAV;
            else if (type == "Sampler")
                return IRResourceTypeSampler;
            else {
                return IRResourceTypeInvalid;
            }
        }
        ShaderType GetShaderType() { return shaderType; }
    private:
        boost::json::value value;
        ShaderType shaderType;
    };

    class HLMetalProgram {
    public:
        HLMetalProgram(std::string name,
                       std::initializer_list<ShaderSource> otherShaders,
                       ShaderConfiguration configuration): name(name) {
            CompileHLSLWithDirectXShaderCompiler(otherShaders);
            std::map<std::string, MTL::Library*> libraries = CompileDXILUsingIR();
            std::map<std::string, MTL::Function*> functions = ExtractFunctions(libraries);
            ReflectLibraryAndCreateState(functions, configuration);
            PrepareDepthStencilState(configuration);

            RELEASEMAP(functions);
            RELEASEMAP(libraries);
        }
    public:
        const std::string& GetName() { return name; }
        MTL::RenderPipelineState* GetState() { return state; }
        MTL::DepthStencilState* GetDepthStencilState() { return depthStencilState; }
        
        std::map<std::string, DXIL::Sampler>& GetSamplers() { return samplers; }
        std::map<std::string, DXIL::Texture>& GetTextures() { return textures; }
        std::map<std::string, DXIL::ConstantBuffer>& GetConstantBuffers() { return constantBuffers; }
        void SetDescriptorTable(uint32_t index, ShaderType type, MTL::RenderCommandEncoder* encoder) {
            auto& pDescriptorTable = pDescriptorTables[type];
            if (pDescriptorTable.size() > index) {
                switch (type) {
                    case ShaderType::Vertex:
                        encoder->setVertexBuffer(pDescriptorTable[index], 0, kIRArgumentBufferBindPoint);
                        break;
                    case ShaderType::Fragment:
                        encoder->setFragmentBuffer(pDescriptorTable[index], 0, kIRArgumentBufferBindPoint);
                        break;
                    default: break;
                }
            }
        }
    public:
        void SetTexture(MTL::Texture* texture, ShaderInput& input, uint32_t objectIndex) {
            for(auto& [key, topLevelArgument]: topLevelArguments) {
                if (topLevelArgument.count(input.GetName()) == 0) continue;
                Argument& resource = topLevelArgument[input.GetName()];
                auto argumentBufferPtr = static_cast<uint8_t*>(pDescriptorTables[resource.GetShaderType()][objectIndex]->contents());
                uint64_t _offset = resource.GetOffset();
                auto entry = reinterpret_cast<IRDescriptorTableEntry*>(argumentBufferPtr + _offset);
                IRDescriptorTableSetTexture(entry, texture, 0, 0);
            }
        }
        
        void CreateDescriptorTableEntries(uint32_t objectIndex) {
            for(auto& [key, value]: topLevelArguments) {
                if (pDescriptorTables[key].size() <= objectIndex) {
                    const uint32_t kNumEntries = (uint32_t)value.size();
                    size_t size= sizeof(IRDescriptorTableEntry) * kNumEntries;
                    
                    if (size == 0) continue;
                    
                    MTL::Buffer* pDescriptorTable = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(size, MTL::ResourceStorageModeShared);
                    auto argumentBufferPtr = static_cast<uint8_t*>(pDescriptorTable->contents());
                    for(auto [skey, svalue]: samplers) {
                        if (value.count(skey) == 0) continue;
                        
                        Argument& resource = value[svalue.GetName()];
                        uint64_t _offset = resource.GetOffset();
                        auto entry = reinterpret_cast<IRDescriptorTableEntry*>(argumentBufferPtr + _offset);
                        
                        MTL::SamplerDescriptor* descriptor = MTL::SamplerDescriptor::alloc()->init();
                        descriptor->setMipFilter(MTL::SamplerMipFilter::SamplerMipFilterLinear);
                        descriptor->setRAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
                        descriptor->setTAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
                        descriptor->setSAddressMode(MTL::SamplerAddressMode::SamplerAddressModeRepeat);
                        descriptor->setMaxAnisotropy(1);
                        descriptor->setCompareFunction(MTL::CompareFunction::CompareFunctionNever);
                        descriptor->setLodMinClamp(0);
                        descriptor->setLodMaxClamp(std::numeric_limits<float>::max());
                        descriptor->setSupportArgumentBuffers(true);
                        IRDescriptorTableSetSampler(entry, MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newSamplerState(descriptor), 0);
                        descriptor->release();
                    }
                    pDescriptorTables[key].push_back(pDescriptorTable);
                }
            }
        }
        
        void SetData(const void* data, std::string& name, uint32_t index, uint32_t size, uint32_t offset, MTL::RenderCommandEncoder* encoder) {
            if (size == 0) return;
            
            for(auto& [key, topLevelArgument]: topLevelArguments) {
                if (topLevelArgument.count(name) == 0) continue;
                
                auto argumentBufferPtr = static_cast<uint8_t*>(pDescriptorTables[key][index]->contents());
                Argument& resource = topLevelArgument[name];
                uint64_t _offset = resource.GetOffset();
                auto entry = reinterpret_cast<IRDescriptorTableEntry*>(argumentBufferPtr + _offset);
                auto& variableBuffer = variableBuffers[key];
                
                MTL::Buffer *passArgument;
                if (variableBuffer.count(name) == 0) {
                    std::vector<MTL::Buffer*> passArgumentsPerObject;
                    passArgument = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(data, size, MTL::ResourceStorageModeShared);
                    passArgumentsPerObject.push_back(passArgument);
                    variableBuffer[name] = passArgumentsPerObject;
                    encoder->useResource(passArgument, MTL::ResourceUsageRead, MTL::RenderStageVertex);
                    encoder->useResource(passArgument, MTL::ResourceUsageRead, MTL::RenderStageFragment);
                } else {
                    std::vector<MTL::Buffer*>& passArgumentsPerObject = variableBuffer[name];
                    if (index >= passArgumentsPerObject.size()) {
                        passArgument = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(data, size, MTL::ResourceStorageModeShared);
                        passArgumentsPerObject.push_back(passArgument);
                        encoder->useResource(passArgument, MTL::ResourceUsageRead, MTL::RenderStageVertex);
                        encoder->useResource(passArgument, MTL::ResourceUsageRead, MTL::RenderStageFragment);
                    } else {
                        passArgument = passArgumentsPerObject[index];
                        memcpy(passArgument->contents(), data, size);
                        encoder->useResource(passArgument, MTL::ResourceUsageRead, MTL::RenderStageVertex);
                        encoder->useResource(passArgument, MTL::ResourceUsageRead, MTL::RenderStageFragment);
                    }
                }
                
                uint64_t gpuAddress = passArgument->gpuAddress();
                uint32_t newOffset = 0;
                
                IRDescriptorTableSetBuffer(entry, gpuAddress, newOffset);
            }
        }
    private:
        void CompileHLSLWithDirectXShaderCompiler(std::initializer_list<ShaderSource> otherShaders) {
            for (ShaderSource otherShader: otherShaders) {
                dxils.push_back(DXIL::Shader(otherShader));
                
                for(auto& dxil: dxils) {
                    for (DXIL::Sampler& sampler: dxil.GetSamplers()) {
                        auto it = samplers.find(sampler.GetName());
                        if (it == samplers.end()) {
                            samplers[sampler.GetName()] = DXIL::Sampler(sampler);
                        }
                    }
                    
                    for (DXIL::Texture& texture: dxil.GetTextures()) {
                        auto it = textures.find(texture.GetName());
                        if (it == textures.end()) {
                            textures[texture.GetName()] = DXIL::Texture(texture);
                        }
                    }
                    
                    for (DXIL::ConstantBuffer& buffer: dxil.GetConstantBuffers()) {
                        auto it = constantBuffers.find(buffer.GetName());
                        if (it == constantBuffers.end()) {
                            constantBuffers[buffer.GetName()] = DXIL::ConstantBuffer(buffer);
                        }
                    }
                }
            }
        }
        
        template<typename T, bool (*reflectFunction)(const IRShaderReflection*, IRReflectionVersion, T*)>
        bool reflectionJSON(const IRShaderReflection* reflection) {
            T info;
            return reflectFunction(reflection, IRReflectionVersion_1_0, &info);
        }
        
        std::map<std::string, MTL::Library*> CompileDXILUsingIR() {
            std::map<std::string, MTL::Library*> libraries;
            
            for(auto& dxil: dxils) {
                ShaderSource& otherShader = dxil.GetShaderSource();
                SmartPoint<IDxcBlob>& pShader = dxil.GetShader();
                const char* entry = otherShader.mainFunction;
                
                IRCompiler* pCompiler = IRCompilerCreate();
                IRCompilerSetEntryPointName(pCompiler, entry);
                IRObject* pDXIL = IRObjectCreateFromDXIL(static_cast<uint8_t*>(pShader->GetBufferPointer()), pShader->GetBufferSize(), IRBytecodeOwnershipCopy);
                
                IRError* pError = nullptr;
                IRObject* pOutIR = IRCompilerAllocCompileAndLink(pCompiler, NULL,  pDXIL, &pError);
                
                if (!pOutIR)
                {
                    IRObjectDestroy(pOutIR);
                    IRObjectDestroy(pDXIL);
                    IRCompilerDestroy(pCompiler);
                    IRErrorDestroy(pError);
                    VWOLF_CORE_ASSERT(static_cast<const char *>(IRErrorGetPayload(pError)));
                }
                
                // Reflection
                IRShaderReflection* reflection = IRShaderReflectionCreate();
                IRObjectGetReflection(pOutIR, ShaderTypeEquivalentIR(otherShader.type), reflection);
                
                std::string jsonString = IRShaderReflectionCopyJSONString(reflection);
                VWOLF_CORE_INFO(jsonString.c_str());
                boost::json::value root = boost::json::parse(jsonString);
                
                boost::json::array arguments = root.at("TopLevelArgumentBuffer").as_array();
                for(auto argument : arguments) {
                    Argument arg(argument, otherShader.type);
                    topLevelArguments[otherShader.type][argument.at("Name").as_string().c_str()] = arg;
                }
                
                if (otherShader.type == ShaderType::Vertex) {
                    boost::json::array vertexInputs = root.at("state").at("vertex_inputs").as_array();
                    for(auto vertexInput : vertexInputs) {
                        stageInAttributes.push_back(StageInAttribute(vertexInput));
                    }
                }
                
                // Clean up
                IRShaderReflectionDestroy(reflection);
                //
                
                // Retrieve Metallib:
                IRMetalLibBinary* pMetallib = IRMetalLibBinaryCreate();
                IRObjectGetMetalLibBinary(pOutIR, ShaderTypeEquivalentIR(otherShader.type), pMetallib);
                
                size_t metallibSize = IRMetalLibGetBytecodeSize(pMetallib);
                uint8_t* metallib = new uint8_t[metallibSize];
                
                IRMetalLibGetBytecode(pMetallib, metallib);

                // Store the metallib to custom format or disk, or use to create a MTLLibrary.
                NS::Error* error = nullptr;
                dispatch_data_t data = dispatch_data_create(metallib, metallibSize, dispatch_get_main_queue(), NULL);
                MTL::Library* library = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newLibrary(data, &error);
                if (error) {
                    IRObjectDestroy(pOutIR);
                    IRObjectDestroy(pDXIL);
                    IRCompilerDestroy(pCompiler);
                    IRErrorDestroy(pError);
                    VWOLF_CORE_ASSERT(error->localizedDescription());
                }

                libraries[dxil.GetName()] = library;
                
                delete [] metallib;
                IRMetalLibBinaryDestroy(pMetallib);
                
                IRObjectDestroy(pOutIR);
                IRObjectDestroy(pDXIL);
                IRCompilerDestroy(pCompiler);
                pCompiler = nullptr;
            }
            return libraries;
        }
        
        std::map<std::string, MTL::Function*> ExtractFunctions(std::map<std::string, MTL::Library*> libraries) {
            std::map<std::string, MTL::Function*> functions;

            for(auto kv: libraries) {
                MTL::Library* library = kv.second;
                NS::UInteger count = library->functionNames()->count();
                for (uint32_t index = 0; index < count; index++) {
                    NS::String* functioName = (NS::String*)library->functionNames()->object(index);
                    functions[functioName->utf8String()] = library->newFunction(functioName);
                }
            }

            return functions;
        }
        
        void ReflectLibraryAndCreateState(std::map<std::string, MTL::Function*> functions,
                                          ShaderConfiguration configuration) {
            MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();

            for(auto kv: functions) {
                MTL::Function* function = kv.second;
                switch(function->functionType()) {
                    case MTL::FunctionType::FunctionTypeVertex:
                        descriptor->setVertexFunction(function);
                        break;
                    case MTL::FunctionTypeFragment:
                        descriptor->setFragmentFunction(function);
                        break;
                    default: break;
                }
            }

            if (name == "Shadow") { // TODO: Move this
                descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatInvalid);
            } else {
                descriptor->colorAttachments()->object(0)->setPixelFormat(MetalDriver::GetCurrent()->GetSurface()->GetPixelFormat());
            }

            PrepareBlending(descriptor->colorAttachments()->object(0), configuration);
            descriptor->setDepthAttachmentPixelFormat(MetalDriver::GetCurrent()->GetSurface()->GetDepthStencilPixelFormat());
            descriptor->setStencilAttachmentPixelFormat(MetalDriver::GetCurrent()->GetSurface()->GetDepthStencilPixelFormat());

            NS::UInteger stride = 0;
            MTL::VertexDescriptor* vertexDescriptor =  MTL::VertexDescriptor::alloc()->init();
            
            for (StageInAttribute& vertexAttribute: stageInAttributes) {
                vertexDescriptor->attributes()->object(kIRStageInAttributeStartIndex + vertexAttribute.GetIndex())->setOffset(stride);
                vertexDescriptor->attributes()->object(kIRStageInAttributeStartIndex + vertexAttribute.GetIndex())->setFormat(vertexAttribute.GetVertexFormat());
                vertexDescriptor->attributes()->object(kIRStageInAttributeStartIndex + vertexAttribute.GetIndex())->setBufferIndex(kIRVertexBufferBindPoint);
                stride += vertexAttribute.GetSize();
            }

            vertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStride(stride);
            vertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStepRate(1);
            vertexDescriptor->layouts()->object(kIRVertexBufferBindPoint)->setStepFunction(MTL::VertexStepFunction::VertexStepFunctionPerVertex);

            descriptor->setVertexDescriptor(vertexDescriptor);

            NS::Error* error = nullptr;
            MTL::RenderPipelineReflection *reflection = nullptr;
            MTL::PipelineOption options = MTL::PipelineOptionArgumentInfo | MTL::PipelineOptionBufferTypeInfo;
            state = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->
            newRenderPipelineState(descriptor, options, &reflection, &error);
            if (!state) {
                VWOLF_CORE_ERROR("Metal shader failed to load: %s.", error->localizedDescription()->utf8String());
                throw std::runtime_error(error->localizedDescription()->utf8String());
            }

            reflection->release();
            descriptor->release();
        }
        
        void PrepareBlending(MTL::RenderPipelineColorAttachmentDescriptor* descriptor, ShaderConfiguration configuration) {
            descriptor->setBlendingEnabled(configuration.blend.enabled);
            descriptor->setRgbBlendOperation(GetOperation(configuration.blend.equation));
            descriptor->setAlphaBlendOperation(GetOperation(configuration.blend.equation));
            descriptor->setSourceRGBBlendFactor(MTL::BlendFactor::BlendFactorSourceAlpha);
            descriptor->setDestinationRGBBlendFactor(MTL::BlendFactor::BlendFactorOneMinusSourceAlpha);
            descriptor->setSourceAlphaBlendFactor(GetFactor(configuration.blend.sourceFunction));
            descriptor->setDestinationAlphaBlendFactor(GetFactor(configuration.blend.destinationFunction));
            descriptor->setWriteMask(MTL::ColorWriteMaskAll);
        }

        MTL::BlendOperation GetOperation(ShaderConfiguration::Blend::Equation equation) {
            switch (equation) {
                case ShaderConfiguration::Blend::Equation::Add:
                    return MTL::BlendOperation::BlendOperationAdd;
                case ShaderConfiguration::Blend::Equation::Substract:
                    return MTL::BlendOperation::BlendOperationSubtract;
                case ShaderConfiguration::Blend::Equation::ReverseSubstract:
                    return MTL::BlendOperation::BlendOperationReverseSubtract;
                case ShaderConfiguration::Blend::Equation::Min:
                    return MTL::BlendOperation::BlendOperationMin;
                case ShaderConfiguration::Blend::Equation::Max:
                    return MTL::BlendOperation::BlendOperationMax;
            }
        }

        MTL::BlendFactor GetFactor(ShaderConfiguration::Blend::Function function) {
            switch (function) {
                case ShaderConfiguration::Blend::Function::Zero:
                    return MTL::BlendFactor::BlendFactorZero;
                case ShaderConfiguration::Blend::Function::One:
                    return MTL::BlendFactor::BlendFactorOne;
                case ShaderConfiguration::Blend::Function::SrcColor:
                    return MTL::BlendFactor::BlendFactorSourceColor;
                case ShaderConfiguration::Blend::Function::InvSrcColor:
                    return MTL::BlendFactor::BlendFactorOneMinusSourceColor;
                case ShaderConfiguration::Blend::Function::DstColor:
                    return MTL::BlendFactor::BlendFactorDestinationColor;
                case ShaderConfiguration::Blend::Function::InvDstColor:
                    return MTL::BlendFactor::BlendFactorOneMinusDestinationColor;
                case ShaderConfiguration::Blend::Function::SrcAlpha:
                    return MTL::BlendFactor::BlendFactorSourceAlpha;
                case ShaderConfiguration::Blend::Function::InvSrcAlpha:
                    return MTL::BlendFactor::BlendFactorOneMinusSourceAlpha;
                case ShaderConfiguration::Blend::Function::DstAlpha:
                    return MTL::BlendFactor::BlendFactorDestinationAlpha;
                case ShaderConfiguration::Blend::Function::InvDstAlpha:
                    return MTL::BlendFactor::BlendFactorOneMinusDestinationAlpha;
                case ShaderConfiguration::Blend::Function::Src1Color:
                    return MTL::BlendFactor::BlendFactorSource1Color;
                case ShaderConfiguration::Blend::Function::InvSrc1Color:
                    return MTL::BlendFactor::BlendFactorOneMinusSource1Color;
                case ShaderConfiguration::Blend::Function::Src1Alpha:
                    return MTL::BlendFactor::BlendFactorSource1Alpha;
                case ShaderConfiguration::Blend::Function::InvSrc1Alpha:
                    return MTL::BlendFactor::BlendFactorOneMinusSource1Alpha;
                case ShaderConfiguration::Blend::Function::SrcAlphaSat:
                    return MTL::BlendFactor::BlendFactorSourceAlphaSaturated;
                case ShaderConfiguration::Blend::Function::CnstColor:
                    return MTL::BlendFactor::BlendFactorBlendColor;
                case ShaderConfiguration::Blend::Function::InvCnstColor:
                    return MTL::BlendFactor::BlendFactorOneMinusBlendColor;
                case ShaderConfiguration::Blend::Function::CnstAlpha:
                    return MTL::BlendFactor::BlendFactorBlendAlpha;
                case ShaderConfiguration::Blend::Function::InvCnstAlpha:
                    return MTL::BlendFactor::BlendFactorOneMinusBlendAlpha;
            }
        }
        
        void PrepareDepthStencilState(ShaderConfiguration configuration) {
            MTL::DepthStencilDescriptor *descriptor = MTL::DepthStencilDescriptor::alloc()->init();

            descriptor->setDepthWriteEnabled(configuration.depthStencil.depthTest);

            switch(configuration.depthStencil.depthFunction) {
                case ShaderConfiguration::DepthStencil::DepthFunction::Never:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionNever);
                    break;
                case ShaderConfiguration::DepthStencil::DepthFunction::Less:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionLess);
                    break;
                case ShaderConfiguration::DepthStencil::DepthFunction::LEqual:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
                    break;
                case ShaderConfiguration::DepthStencil::DepthFunction::Equal:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionEqual);
                    break;
                case ShaderConfiguration::DepthStencil::DepthFunction::NotEqual:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionNotEqual);
                    break;
                case ShaderConfiguration::DepthStencil::DepthFunction::GEqual:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionGreaterEqual);
                    break;
                case ShaderConfiguration::DepthStencil::DepthFunction::Greater:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionGreater);
                    break;
                case ShaderConfiguration::DepthStencil::DepthFunction::Always:
                    descriptor->setDepthCompareFunction(MTL::CompareFunctionAlways);
                    break;
            }
            depthStencilState = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newDepthStencilState(descriptor);
        }
    private:
        std::string name;
        std::vector<DXIL::Shader> dxils;
        std::vector<StageInAttribute> stageInAttributes;
        std::map<ShaderType, std::map<std::string, Argument>> topLevelArguments;
        
        MTL::RenderPipelineState* state;
        std::map<ShaderType, std::vector<MTL::Buffer*>> pDescriptorTables;
        MTL::DepthStencilState *depthStencilState = nullptr;
        
        // DXIL
        std::map<std::string, DXIL::Sampler> samplers;
        std::map<std::string, DXIL::Texture> textures;
        std::map<std::string, DXIL::ConstantBuffer> constantBuffers;
        std::map<ShaderType, std::map<std::string, std::vector<MTL::Buffer*>>> variableBuffers;
    };

    HLSLMetalShader::HLSLMetalShader(std::string name,
                                     std::initializer_list<ShaderSource> otherShaders,
                                     ShaderConfiguration configuration):
    MetalShader(name, otherShaders, configuration) {
        hlMetalProgram = CreateRef<HLMetalProgram>(name, otherShaders, configuration);
    }

    HLSLMetalShader::~HLSLMetalShader() {
        
    }

    void HLSLMetalShader::Bind() const {
        encoder->setRenderPipelineState(hlMetalProgram->GetState());
        SetConfiguration();
    }

    void HLSLMetalShader::Unbind() const {
        
    }

    std::string HLSLMetalShader::GetName() const {
        return this->m_name;
    }

    void HLSLMetalShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
        std::string cbName = std::string("cbPer") + std::string(name);
        hlMetalProgram->SetData(data, cbName, currentIndex, size, offset, encoder);
    }

    std::vector<Ref<ShaderInput>> HLSLMetalShader::GetMaterialInputs() const {
        std::string cbMaterialName = std::string("cbPer") + std::string(materialName);
        
        if (hlMetalProgram->GetConstantBuffers().count(cbMaterialName) == 0) return {};
        
        DXIL::ConstantBuffer& material = hlMetalProgram->GetConstantBuffers()[cbMaterialName];
        
        std::vector<Ref<ShaderInput>> inputs;

        for (DXIL::ConstantBufferVariable variable : material.GetVariables()) {
            if (variable.GetShaderDataType() == ShaderDataType::None) continue;
            inputs.push_back(CreateRef<ShaderInput>(variable.GetName(),
                variable.GetShaderDataType(),
                variable.GetIndex(),
                ShaderDataTypeSize(variable.GetShaderDataType()),
                variable.GetOffset()));
        }
        return inputs;
    }

    size_t HLSLMetalShader::GetMaterialSize() const {
        std::string cbMaterialName = std::string("cbPer") + std::string(materialName);
        
        if (hlMetalProgram->GetConstantBuffers().count(cbMaterialName) != 0) {
            return hlMetalProgram->GetConstantBuffers()[cbMaterialName].GetSize();
        }
        return 0;
    }

    std::vector<ShaderInput> HLSLMetalShader::GetTextureInputs() const {
        std::vector<ShaderInput> inputs;

        for (auto [key, variable] : hlMetalProgram->GetTextures()) {
            inputs.push_back(ShaderInput(variable.GetName(),
                                         variable.GetShaderDataType(),
                                         (uint32_t)variable.GetBindingIndex(),
                                         (uint32_t)variable.GetTextureType(),
                                         0));
        }
        return inputs;
    }

    MTL::RenderPipelineState* HLSLMetalShader::GetState() {
        return hlMetalProgram->GetState();
    }

    void HLSLMetalShader::SetObjectIndex(uint32_t index) {
        currentIndex = index;
        if (hlMetalProgram->GetConstantBuffers().size() > 0) {
            hlMetalProgram->CreateDescriptorTableEntries(index);
            hlMetalProgram->SetDescriptorTable(index, ShaderType::Vertex, encoder);
            hlMetalProgram->SetDescriptorTable(index, ShaderType::Fragment, encoder);
        }
    }

    void HLSLMetalShader::SetVertexBufferIndex(Ref<MetalVertexBuffer> buffer) {
        encoder->setVertexBuffer(*buffer, 0, kIRVertexBufferBindPoint);
    }

    void HLSLMetalShader::SetTextures(Ref<MetalRenderTexture> shadowMap, Material& material)  {
        for (auto textureInput : GetTextureInputs()) {
            if (textureInput.GetName() == "Shadow") {
                encoder->useResource(reinterpret_cast<MTL::Texture*>(shadowMap->GetHandler()), MTL::ResourceUsageRead, MTL::RenderStageFragment);
                hlMetalProgram->SetTexture(reinterpret_cast<MTL::Texture*>(shadowMap->GetHandler()), textureInput, currentIndex);
            } else {
                Ref<Texture> texture = material.GetTexture(textureInput.GetName());
                if (texture != nullptr) {
                    encoder->useResource(reinterpret_cast<MTL::Texture*>(texture->GetHandler()), MTL::ResourceUsageRead, MTL::RenderStageFragment);
                    hlMetalProgram->SetTexture(reinterpret_cast<MTL::Texture*>(texture->GetHandler()), textureInput, currentIndex);
                }
            }
        }
    }

    void HLSLMetalShader::Draw(MTL::PrimitiveType type, Ref<MetalIndexBuffer> buffer) {        
//        IRRuntimeDrawPrimitives(encoder, MTL::PrimitiveTypeTriangle, 0, 6, 1, 0 );
        IRRuntimeDrawIndexedPrimitives(encoder, type, buffer->GetCount(), buffer->GetType(), *buffer, 0);
    }

    void HLSLMetalShader::UseShader(MTL::RenderCommandEncoder* encoder) {
        this->encoder = encoder;
    }

    void HLSLMetalShader::SetConfiguration() const {
        SetRasterization();
        SetDepthStencil();
    }

    void HLSLMetalShader::SetRasterization() const {
        if (m_configuration.rasterization.cullEnabled) {
            switch (m_configuration.rasterization.cullMode) {
                case ShaderConfiguration::Rasterization::CullMode::Front:
                    encoder->setCullMode(MTL::CullMode::CullModeFront);
                    break;
                case ShaderConfiguration::Rasterization::CullMode::Back:
                    encoder->setCullMode(MTL::CullMode::CullModeBack);
                    break;
                case ShaderConfiguration::Rasterization::CullMode::FrontAndBack:
                    encoder->setCullMode(MTL::CullMode::CullModeNone);
                    break;
            }
        } else
            encoder->setCullMode(MTL::CullMode::CullModeNone);
        switch (m_configuration.rasterization.fillMode) {
            case ShaderConfiguration::Rasterization::FillMode::Solid:
                encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeFill);
                break;
            case ShaderConfiguration::Rasterization::FillMode::Wireframe:
                encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeLines);
                break;
        }
    }

    void HLSLMetalShader::SetBlend() const {
        
    }

    void HLSLMetalShader::SetDepthStencil() const {
        if (hlMetalProgram->GetDepthStencilState() == nullptr) return;
        encoder->setDepthStencilState(hlMetalProgram->GetDepthStencilState());
    }

}

#endif
