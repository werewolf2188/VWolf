//
//  MSLShader.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/25/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MSLShader.h"

#include "VWolf/Core/Utils/File.h"
#include "VWolf/Platform/Metal/MetalDriver.h"

#define RELEASEMAP(map) \
for(auto kv: map) \
kv.second->release();

namespace VWolf {

    MTL::VertexFormat TransformFormat(MTL::DataType from) {
        switch (from) {
            case MTL::DataTypeFloat:
                return MTL::VertexFormat::VertexFormatFloat;
            case MTL::DataTypeFloat2:
                return MTL::VertexFormat::VertexFormatFloat2;
            case MTL::DataTypeFloat3:
                return MTL::VertexFormat::VertexFormatFloat3;
            case MTL::DataTypeFloat4:
                return MTL::VertexFormat::VertexFormatFloat4;
            case MTL::DataTypeFloat4x4:
                return MTL::VertexFormat::VertexFormatFloat4;
            default: break;
        }
        return MTL::VertexFormatInvalid;
    }

    ShaderDataType FromMLToVWolfDataType(MTL::DataType from) {
        switch (from) {
            case MTL::DataTypeFloat:
                return ShaderDataType::Float;
            case MTL::DataTypeFloat2:
                return ShaderDataType::Float2;
            case MTL::DataTypeFloat3:
                return ShaderDataType::Float3;
            case MTL::DataTypeFloat4:
                return ShaderDataType::Float4;
            case MTL::DataTypeFloat3x3:
                return ShaderDataType::Mat3;
            case MTL::DataTypeFloat4x4:
                return ShaderDataType::Mat4;
            case MTL::DataTypeInt:
                return ShaderDataType::Int;
            case MTL::DataTypeInt2:
                return ShaderDataType::Int2;
            case MTL::DataTypeInt3:
                return ShaderDataType::Int3;
            case MTL::DataTypeInt4:
                return ShaderDataType::Int4;
            case MTL::DataTypeBool:
                return ShaderDataType::Bool;
            default: break;
        }
        return ShaderDataType::None;
    }

    uint64_t SizeFrom(MTL::VertexFormat format) {
        
        switch (format) {
            case MTL::VertexFormat::VertexFormatFloat: return sizeof(float);
            case MTL::VertexFormat::VertexFormatFloat2: return sizeof(float) * 2;
            case MTL::VertexFormat::VertexFormatFloat3: return sizeof(float) * 3;
            case MTL::VertexFormat::VertexFormatFloat4: return sizeof(float) * 4;
            default: break;
        }
        return -1;
    }

    uint64_t SizeFrom(MTL::DataType format) {
        
        switch (format) {
            case MTL::DataTypeFloat: return sizeof(float);
            case MTL::DataTypeFloat2: return sizeof(float) * 2;
            case MTL::DataTypeFloat3: return sizeof(float) * 3;
            case MTL::DataTypeFloat4: return sizeof(float) * 4;
            case MTL::DataTypeFloat4x4: return sizeof(float) * 4 * 4;
            default: break;
        }
        return -1;
    }
    
    class MLAttribute {
    public:
        MLAttribute(MTL::Attribute* attribute) {
            format = attribute->attributeType();
            index = attribute->attributeIndex();
            name = attribute->name()->utf8String();
        }
    public:
        MTL::DataType GetFormat() { return format; }

        uint64_t GetIndex() { return index; }

        MTL::VertexFormat GetVertexFormat() { return TransformFormat(format); }

        uint64_t GetSize() { return SizeFrom(GetVertexFormat()); }

        std::string GetName() { return name; }
    private:
        MTL::DataType format;
        uint64_t index;
        std::string name;
    };

    class MLConstantVariable {
    public:
        MLConstantVariable(MTL::StructMember* member) {
            name = member->name()->utf8String();
            format = member->dataType();
            index = member->argumentIndex();
            offset = member->offset();
        }
    public:
        MTL::DataType GetFormat() { return format; }
        uint64_t GetIndex() { return index; }
        std::string GetName() { return name; }
        uint64_t GetSize() { return SizeFrom(format); }
        uint64_t GetOffset() { return offset; }
    private:
        MTL::DataType format;
        uint64_t index;
        std::string name;
        uint64_t offset;
    };

    class MLConstantBuffer {
    public:
        MLConstantBuffer(MTL::Argument* argument) {
            name = argument->name()->utf8String();
            format = argument->bufferDataType();
            size = argument->bufferDataSize();
            index = argument->index();

            MTL::StructType* structType = argument->bufferStructType();

            NS::Array* members = structType->members();
            NS::UInteger memberCount = members->count();

            for (uint32_t index = 0; index < memberCount; index++) {                
                MTL::StructMember* member = (MTL::StructMember*)members->object(index);
                constantVariables[member->name()->utf8String()] = CreateRef<MLConstantVariable>(member);
            }
        }

        ~MLConstantBuffer() {
            for(auto uniform: uniforms)
                uniform->release();
        }
    public:
        MTL::DataType GetFormat() { return format; }
        uint64_t GetIndex() { return index; }
        std::string GetName() { return name; }
        uint64_t GetSize() { return size; }
        std::map<std::string, Ref<MLConstantVariable>> GetConstantVariables() { return constantVariables; }
    public:
        void SetData(const void* data, uint32_t size, uint32_t index) {
            if (index >= uniforms.size()) {
                uniforms.push_back(MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(data, size, {}));
            } else {
                memcpy(uniforms[index]->contents(), data, size);
            }
        }

        MTL::Buffer * GetUniform(uint32_t index) {
            return uniforms[index];
        }
    private:
        MTL::DataType format;
        uint64_t index;
        std::string name;
        uint64_t size;
        std::vector<MTL::Buffer*> uniforms;
        std::map<std::string, Ref<MLConstantVariable>> constantVariables;
    };

    class MLProgram {
    public:
        MLProgram(std::string name,
                  std::initializer_list<ShaderSource> otherShaders,
                  ShaderConfiguration configuration = {}): name(name), configuration(configuration) {
    
            std::map<std::string, MTL::Library*> libraries = CompileLibraries(otherShaders);
            std::map<std::string, MTL::Function*> functions = ExtractFunctions(otherShaders, libraries);

            ReflectLibraryAndCreateState(functions);

            RELEASEMAP(functions);
            RELEASEMAP(libraries);
        }

        ~MLProgram() {
            state->release();
        }
    private:
        std::map<std::string, MTL::Library*> CompileLibraries(std::initializer_list<ShaderSource> otherShaders) {
            std::map<std::string, MTL::Library*> libraries;

            for (ShaderSource otherShader: otherShaders) {
                if (libraries.count(otherShader.shader) == 0) {
                    const std::string file = File::OpenTextFile(otherShader.shader.c_str());
                    const NS::String* nsFile = NSSTRINGFROM(file.c_str());
                    NS::Error* error = nullptr;
                    MTL::CompileOptions* compileOptions = nullptr;
                    MTL::Library* library = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newLibrary(nsFile, compileOptions, &error);
                    if (!library) {
                        VWOLF_CORE_ERROR("Metal shader failed to load: %s.", error->localizedDescription()->utf8String());
                        throw std::runtime_error(error->localizedDescription()->utf8String());
                    }
                    libraries[otherShader.shader] = library;
                }
            }

            return libraries;
        }

        std::map<std::string, MTL::Function*> ExtractFunctions(std::initializer_list<ShaderSource> otherShaders, 
                                                               std::map<std::string, MTL::Library*> libraries) {
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

        void ReflectLibraryAndCreateState(std::map<std::string, MTL::Function*> functions) {
            MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();

            for(auto kv: functions) {
                MTL::Function* function = kv.second;
                switch(function->functionType()) {
                    case MTL::FunctionType::FunctionTypeVertex:
                        descriptor->setVertexFunction(function);
                        PrepareAttributes(function);
                        break;
                    case MTL::FunctionTypeFragment:
                        descriptor->setFragmentFunction(function);
                        break;
                    default: break;
                }
            }

            descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

            NS::UInteger stride = 0;
            MTL::VertexDescriptor* vertexDescriptor =  MTL::VertexDescriptor::alloc()->init();

            for (auto kv: vertexAttributes) {
                Ref<MLAttribute> vertexAttribute = kv.second;
                vertexDescriptor->attributes()->object(vertexAttribute->GetIndex())->setOffset(stride);
                vertexDescriptor->attributes()->object(vertexAttribute->GetIndex())->setFormat(vertexAttribute->GetVertexFormat());
                stride += vertexAttribute->GetSize();
            }

            vertexDescriptor->layouts()->object(0)->setStride(stride);

            descriptor->setVertexDescriptor(vertexDescriptor);

            NS::Error* error = nullptr;
            MTL::RenderPipelineReflection *reflection = nullptr;
            MTL::PipelineOption options = MTL::PipelineOptionArgumentInfo | MTL::PipelineOptionBufferTypeInfo;
            state = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->
            newRenderPipelineState(descriptor, options, &reflection, &error);

            PrepareBindings(reflection);

            reflection->release();
            descriptor->release();
        }

        void PrepareAttributes(MTL::Function* function) {
            if (function->functionType() != MTL::FunctionType::FunctionTypeVertex) {
                VWOLF_CORE_ERROR("Sending wrong function.");
                throw std::runtime_error("Metal function is not vertex function");
            }

            NS::Array* vertexAttributes = function->stageInputAttributes();
            NS::UInteger vertextAttributesCount = vertexAttributes->count();

            for (int index = 0; index < vertextAttributesCount; index++) {
                MTL::Attribute* vertexAttribute = (MTL::Attribute*)vertexAttributes->object(index);
                this->vertexAttributes[vertexAttribute->attributeIndex()] = CreateRef<MLAttribute>(vertexAttribute);
            }
        }

        void PrepareBindings(MTL::RenderPipelineReflection *reflection) {
            NS::Array* vertexArguments = reflection->vertexArguments();
            NS::UInteger vertexArgumentsCount = vertexArguments->count();

            for (uint32_t index = 0; index < vertexArgumentsCount; index++) {
                MTL::Argument* argument = (MTL::Argument*)vertexArguments->object(index);
                if (argument->bufferStructType() != nullptr) {
                    constantBuffers[argument->name()->utf8String()] = CreateRef<MLConstantBuffer>(argument);
                }
            }
        }
    public:
        MTL::RenderPipelineState* GetState() { return state; }
        std::map<NS::UInteger, Ref<MLAttribute>> GetVertexAttributes() { return vertexAttributes; }
        std::map<std::string, Ref<MLConstantBuffer>> GetConstantBuffers() { return constantBuffers; }
    private:
        std::string name;
        ShaderConfiguration configuration;
        // Required variables
        MTL::RenderPipelineState* state;
        std::map<NS::UInteger, Ref<MLAttribute>> vertexAttributes;
        std::map<std::string, Ref<MLConstantBuffer>> constantBuffers;
    };

    MSLShader::MSLShader(std::string name,
                         std::initializer_list<ShaderSource> otherShaders,
                         ShaderConfiguration configuration): 
    Shader(name, otherShaders, configuration) {

        mlProgram = CreateRef<MLProgram>(name, otherShaders, configuration);
    }

    MSLShader::~MSLShader() {
        
    }

    MTL::RenderPipelineState* MSLShader::GetState() {
        return mlProgram->GetState();
        
    }

    void MSLShader::UseShader(MTL::RenderCommandEncoder* encoder) {
        this->encoder = encoder;
    }

    void MSLShader::SetConfiguration() const {
        SetRasterization();
    }

    void MSLShader::SetRasterization() const {
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
    }

    void MSLShader::SetBlend() const {
        
    }

    void MSLShader::SetDepthStencil() const {
        
    }

    void MSLShader::Bind() const {
        encoder->setRenderPipelineState(mlProgram->GetState());
        SetConfiguration();
    }

    void MSLShader::Unbind() const {
        
    }

    std::string MSLShader::GetName() const {
        return m_name;
    }

    void MSLShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
        std::string strName = name;
        boost::to_lower(strName);
        if (mlProgram->GetConstantBuffers().count(strName) == 0) return;

        Ref<MLConstantBuffer> constantBuffer = mlProgram->GetConstantBuffers()[strName];
        constantBuffer->SetData(data, size, offset);
        NS::UInteger uniformIndex = constantBuffer->GetIndex();
        encoder->setVertexBuffer(constantBuffer->GetUniform(offset), 0, uniformIndex);
    }

    NS::UInteger MSLShader::GetIndex(const char* name) {
        std::string strName = name;
        boost::to_lower(strName);
        if (mlProgram->GetConstantBuffers().count(strName) == 0) return -1;

        return mlProgram->GetConstantBuffers()[name]->GetIndex();
    }

    NS::UInteger MSLShader::GetVertexBufferIndex() {
        return 0;
    }

    std::vector<Ref<ShaderInput>> MSLShader::GetMaterialInputs() const {
        if (mlProgram->GetConstantBuffers().count(materialName) != 0) {
            std::vector<Ref<ShaderInput>> materialInputs;

            Ref<MLConstantBuffer> materialConstantBuffer = mlProgram->GetConstantBuffers()[materialName];

            for (auto kv: materialConstantBuffer->GetConstantVariables()) {
                Ref<MLConstantVariable> variable = kv.second;
                Ref<ShaderInput> materialInput = CreateRef<ShaderInput>(
                                                                        variable->GetName(),
                                                                        FromMLToVWolfDataType(variable->GetFormat()),
                                                                        variable->GetIndex(),
                                                                        variable->GetSize(),
                                                                        variable->GetOffset()
                                                                        );
                materialInputs.push_back(materialInput);
            }
        }
        return std::vector<Ref<ShaderInput>>();
    }

    size_t MSLShader::GetMaterialSize() const {
        if (mlProgram->GetConstantBuffers().count(materialName) != 0) {
            Ref<MLConstantBuffer> materialConstantBuffer = mlProgram->GetConstantBuffers()[materialName];
            return materialConstantBuffer->GetSize();
        }
        return 0;
    }

    std::vector<ShaderInput> MSLShader::GetTextureInputs() const {
        return std::vector<ShaderInput>();
    }
}

#endif
