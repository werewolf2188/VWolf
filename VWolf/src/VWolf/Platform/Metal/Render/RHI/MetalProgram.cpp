//
//  MetalProgram.cpp
//  VWolf
//
//  Created by Enrique Moises on 9/2/26.
//
#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "VWolf/Platform/DXILHelpers.h"
#include "MetalProgram.h"
#include "MetalShader.h"
#include "MetalDriver.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

#define IR_RUNTIME_METALCPP
#include <metal_irconverter_runtime/metal_irconverter_runtime.h>

namespace VWolf::RHI {

    namespace {
        static IRShaderStage ShaderTypeEquivalentIR(VWolf::ShaderType type) {
            switch (type) {
                case VWolf::ShaderType::Vertex: return IRShaderStageVertex;
                case VWolf::ShaderType::Fragment: return IRShaderStageFragment;
                case VWolf::ShaderType::Pre_Tesselator: return IRShaderStageHull;
                case VWolf::ShaderType::Post_Tesselator: return IRShaderStageDomain;
                case VWolf::ShaderType::Geometry: return IRShaderStageGeometry;
                case VWolf::ShaderType::Compute: return IRShaderStageCompute;
            }
        }
    
        static uint64_t GetSizeFrom(MTL::VertexFormat format) {
            
            switch (format) {
                case MTL::VertexFormat::VertexFormatFloat: return sizeof(float);
                case MTL::VertexFormat::VertexFormatFloat2: return sizeof(float) * 2;
                case MTL::VertexFormat::VertexFormatFloat3: return sizeof(float) * 3;
                case MTL::VertexFormat::VertexFormatFloat4: return sizeof(float) * 4;
                default: break;
            }
            return -1;
        }
    }

    Argument::Argument(): value(nullptr), shaderType((ShaderType)0) { }

    Argument::Argument(boost::json::value value, ShaderType shaderType): value(value), shaderType(shaderType) { }

    Argument::Argument(const Argument& argument): value(argument.value), shaderType(argument.shaderType) { }

    Argument::Argument(Argument&& argument): value(argument.value), shaderType(argument.shaderType) {
        argument.value = nullptr;
        argument.shaderType = (ShaderType)0;
    }

    Argument& Argument::operator=(const Argument& argument) {
        this->value = argument.value;
        this->shaderType = argument.shaderType;
        return *this;
    }

    uint64_t Argument::GetIndex() { return value.at("Slot").as_uint64(); }
    
    uint64_t Argument::GetSpace() { return value.at("Space").as_uint64(); }
    
    std::string Argument::GetName() { return value.at("Name").as_string().c_str();}
    
    uint64_t Argument::GetSize() { return value.at("Size").as_uint64(); }
    
    int64_t Argument::GetOffset() { return value.at("EltOffset").as_int64(); }
    
    IRResourceType Argument::GetType() {
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
    
    ShaderType Argument::GetShaderType() { return shaderType; }

    StageInAttribute::StageInAttribute(boost::json::value value): value(value) { }

    int64_t StageInAttribute::GetIndex() { return value.at("index").as_int64(); }

    MTL::VertexFormat StageInAttribute::GetVertexFormat() {
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

    AttributeFormat StageInAttribute::GetAttributeFormat() {
        std::string elementType = value.at("elementType").as_string().c_str();
        int64_t columnCount = value.at("columnCount").as_int64();
        if (elementType == "Float") return AttributeFormat::Float32;
        else if (elementType == "Int") return AttributeFormat::SInt32;
        return AttributeFormat::Unknown;
    }

    uint32_t StageInAttribute::GetDimension() {
        switch (GetVertexFormat()) {
            case MTL::VertexFormat::VertexFormatFloat: return 1;
            case MTL::VertexFormat::VertexFormatFloat2: return 2;
            case MTL::VertexFormat::VertexFormatFloat3: return 3;
            case MTL::VertexFormat::VertexFormatFloat4: return 4;
            case MTL::VertexFormat::VertexFormatInt: return 1;
            case MTL::VertexFormat::VertexFormatInt2: return 2;
            case MTL::VertexFormat::VertexFormatInt3: return 3;
            case MTL::VertexFormat::VertexFormatInt4: return 4;
            case MTL::VertexFormat::VertexFormatUInt: return 1;
            default: return -1;
        }
    }

    std::string StageInAttribute::GetName() { return value.at("name").as_string().c_str(); }

    uint64_t StageInAttribute::GetSize() { return GetSizeFrom(GetVertexFormat()); }


    MetalProgram::MetalProgram(Weak<Context> context, const ProgramSettings& settings): Program(context, settings) {
        ExtractFunctions(CompileDXILUsingIR());
    }

    MetalProgram::~MetalProgram() {}

    void MetalProgram::SetName(const std::string& name) {
        Object::SetName(name);
        
        for(const auto& [key, value]: functions) {
            std::string compositeName = name + "." + key;
            value->setLabel(NSSTRINGFROM(compositeName.data()));
        }
    }

    std::map<std::string, Ref<MTL::Library>> MetalProgram::CompileDXILUsingIR() {
        std::map<std::string, Ref<MTL::Library>> libraries;
        
        for(const Ref<Shader>& shader: GetSettings().GetShaders()) {
            const Ref<MetalShader> metalShader = DynamicAs<MetalShader>(shader);
            const Ref<VWolf::DXIL::Shader>& dxil = metalShader->GetDXILCompiledShader();
            
            SmartPoint<IDxcBlob>& pShader = dxil->GetShader();
            const char* entry = dxil->GetMainFunction();
            
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
            IRObjectGetReflection(pOutIR, ShaderTypeEquivalentIR(dxil->GetType()), reflection);
            
            std::string jsonString = IRShaderReflectionCopyJSONString(reflection);
            boost::json::value root = boost::json::parse(jsonString);
            
            boost::json::array arguments = root.at("TopLevelArgumentBuffer").as_array();
            for(auto argument : arguments) {
                Ref<Argument> arg = CreateRef<Argument>(argument, (ShaderType)(uint32_t)dxil->GetType());
                topLevelArguments[(ShaderType)(uint32_t)dxil->GetType()][argument.at("Name").as_string().c_str()] = arg;
            }
            
            if (dxil->GetType() == VWolf::ShaderType::Vertex) {
                boost::json::array vertexInputs = root.at("state").at("vertex_inputs").as_array();
                for(auto vertexInput : vertexInputs) {
                    stageInAttributes.push_back(CreateRef<StageInAttribute>(vertexInput));
                }
            }
            
            // Clean up
            IRShaderReflectionDestroy(reflection);
            //
            
            // Retrieve Metallib:
            IRMetalLibBinary* pMetallib = IRMetalLibBinaryCreate();
            IRObjectGetMetalLibBinary(pOutIR, ShaderTypeEquivalentIR(dxil->GetType()), pMetallib);
            
            size_t metallibSize = IRMetalLibGetBytecodeSize(pMetallib);
            uint8_t* metallib = new uint8_t[metallibSize];
            
            IRMetalLibGetBytecode(pMetallib, metallib);

            // Store the metallib to custom format or disk, or use to create a MTLLibrary.
            NS::Error* error = nullptr;
            dispatch_data_t data = dispatch_data_create(metallib, metallibSize, dispatch_get_main_queue(), NULL);
            Ref<MTL::Library> library = CreateMetalRef<MTL::Library>(MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newLibrary(data, &error));
            if (error) {
                IRObjectDestroy(pOutIR);
                IRObjectDestroy(pDXIL);
                IRCompilerDestroy(pCompiler);
                IRErrorDestroy(pError);
                VWOLF_CORE_ASSERT(error->localizedDescription());
            }

            libraries[dxil->GetName()] = library;
            
            delete [] metallib;
            IRMetalLibBinaryDestroy(pMetallib);
            
            IRObjectDestroy(pOutIR);
            IRObjectDestroy(pDXIL);
            IRCompilerDestroy(pCompiler);
            pCompiler = nullptr;
        }
        
        return libraries;
    }

    void MetalProgram::ExtractFunctions(std::map<std::string, Ref<MTL::Library>> libraries) {
        
        for(auto kv: libraries) {
            Ref<MTL::Library> library = kv.second;
            NS::UInteger count = library->functionNames()->count();
            for (uint32_t index = 0; index < count; index++) {
                NS::String* functioName = (NS::String*)library->functionNames()->object(index);
                functions[functioName->utf8String()] = CreateMetalRef<MTL::Function>(library->newFunction(functioName));
            }
        }
    }
}
#endif
