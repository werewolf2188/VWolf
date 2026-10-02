//
//  Program.h
//  VWolf
//
//  Created by Enrique Moises on 9/1/26.
//

#pragma once

#include "Shader.h"
#include "Types.h"
#include "Texture.h"
#include "Object.h"

namespace VWolf::RHI {

    class ProgramInputBuffer {
    public:
        ProgramInputBuffer() = default;
        ProgramInputBuffer(
                           const std::string name,
                           const uint32_t index,
                           const PixelFormat format,
                           const uint32_t offset,
                           const uint32_t numberOfElements,
                           const uint8_t elementType
                           ):
        name(name), index(index), format(format), offset(offset), numberOfElements(numberOfElements), elementType(elementType) {}
        
        const std::string& GetName() const { return name; }
        const uint32_t& GetIndex() const { return index; }
        const PixelFormat& GetFormat() const { return format; }
        const uint32_t& GetOffset() const { return offset; }
        const uint32_t& GetNumberOfElements() const { return numberOfElements; }
        const uint8_t& GetElementType() const { return elementType; }
    private:
        std::string name;
        uint32_t index;
        PixelFormat format;
        uint32_t offset;        
        uint32_t numberOfElements;
        uint8_t elementType;
    };

    class ProgramArgument {
    public:
        ProgramArgument(ShaderType shaderType,
                        std::string name): shaderType(shaderType), name(name) {}
        
        virtual ResourceType GetResourceType() = 0;
        
        const ShaderType& GetShaderType() const { return shaderType; }
        const std::string& GetName() const { return name; }
    protected:
        ShaderType shaderType;
        std::string name;
    };

    enum class ProgramArgumentAccessType
    {
        Constant,
        FrameConstant,
        Mutable
    };

    enum class ProgramArgumentValueType
    {
        ResourceView,       // Default argument access by descriptor from resource view
        BufferAddress,      // GPU addressable buffer view with offset and size
        RootConstantBuffer, // Root constant stored in the program-managed buffer and referenced by GPU address
        RootConstantValue   // Root constant value stored in the root signature as 32-bit values
    };

    class BindableProgramArgument: public ProgramArgument {
    public:
        struct Hash {
            std::size_t operator()(const Ref<BindableProgramArgument> arg) const {
                std::hash<std::string> hasher;
                return hasher(arg->GetName());
            }
        };
        
        BindableProgramArgument(ShaderType shaderType,
                                std::string name,
                                uint32_t bindingPoint,
                                ProgramArgumentAccessType accessType = ProgramArgumentAccessType::Mutable,
                                ProgramArgumentValueType valueType = ProgramArgumentValueType::ResourceView):
        ProgramArgument(shaderType, name), bindingPoint(bindingPoint), accessType(accessType), valueType(valueType) {}
        
        const uint32_t& GetBindingPoint() const { return bindingPoint; }
        const ProgramArgumentAccessType& GetAccessorType() const { return accessType; }
        const ProgramArgumentValueType& GetValueType() const { return valueType; }
        const bool IsResourceView() const { return valueType == ProgramArgumentValueType::ResourceView; }
        const bool IsAddressable() const { return valueType == ProgramArgumentValueType::BufferAddress; }
        const bool IsRootConstantBuffer() const { return valueType == ProgramArgumentValueType::RootConstantBuffer; }
        const bool IsRootConstantValue() const { return valueType == ProgramArgumentValueType::RootConstantValue; }
        const bool IsRootConstant() const { return IsRootConstantBuffer() || IsRootConstantValue(); }
        const bool IsMutable() const { return accessType == ProgramArgumentAccessType::Mutable; }
        const bool IsConstant() const { return accessType == ProgramArgumentAccessType::Constant; }
        const bool IsFrameConstant() const { return accessType == ProgramArgumentAccessType::FrameConstant; }
    protected:
        uint32_t bindingPoint = -1;
        ProgramArgumentAccessType accessType = ProgramArgumentAccessType::Mutable;
        ProgramArgumentValueType valueType = ProgramArgumentValueType::ResourceView;
    };

    class TextureProgramArgument: public BindableProgramArgument {
    public:
        TextureProgramArgument(ShaderType shaderType,
                               std::string name,
                               uint32_t bindingPoint,
                               TextureDimensionType textureDimensionType,
                               ProgramArgumentAccessType accessType = ProgramArgumentAccessType::Mutable,
                               ProgramArgumentValueType valueType = ProgramArgumentValueType::ResourceView):
        BindableProgramArgument(shaderType, name, bindingPoint, accessType, valueType), textureDimensionType(textureDimensionType) {}
        
        virtual ResourceType GetResourceType() override { return ResourceType::Texture; }
        
        const TextureDimensionType& GetTextureDimensionType() const { return textureDimensionType; }
    private:
        TextureDimensionType textureDimensionType;
    };

    class SamplerProgramArgument: public BindableProgramArgument {
    public:
        SamplerProgramArgument(ShaderType shaderType,
                               std::string name,
                               uint32_t bindingPoint,
                               uint32_t numSamples,
                               ProgramArgumentAccessType accessType = ProgramArgumentAccessType::Mutable,
                               ProgramArgumentValueType valueType = ProgramArgumentValueType::ResourceView):
        BindableProgramArgument(shaderType, name, bindingPoint, accessType, valueType), numSamples(numSamples) {}
        
        virtual ResourceType GetResourceType() override { return ResourceType::Sampler; }
        
        const uint32_t& GetNumSamples() const { return numSamples; }
    private:
        uint32_t numSamples;
    };

    class ConstantBufferVariableProgramArgument: public ProgramArgument {
    public:
        
        ConstantBufferVariableProgramArgument(ShaderType shaderType,
                                              std::string name,
                                              size_t size,
                                              uint32_t offset,
                                              uint32_t index,
                                              DataType dataType):
        ProgramArgument(shaderType, name), size(size), offset(offset), index(index), dataType(dataType) {}
        
        virtual ResourceType GetResourceType() override { return ResourceType::Buffer; }
        
        const size_t& GetSize() const { return size; }
        const uint32_t& GetOffset() const { return offset; }
        const uint32_t& GetIndex() const { return index; }
        const DataType& GetDataType() const { return dataType; }
    private:
        size_t size;
        uint32_t offset;
        uint32_t index;
        DataType dataType;
    };

    class ConstantBufferProgramArgument: public BindableProgramArgument {
    public:
        
        ConstantBufferProgramArgument(ShaderType shaderType,
                                      std::string name,
                                      uint32_t bindingPoint,
                                      size_t size,
                                      std::vector<ConstantBufferVariableProgramArgument> variables,
                                      ProgramArgumentAccessType accessType = ProgramArgumentAccessType::Mutable,
                                      ProgramArgumentValueType valueType = ProgramArgumentValueType::ResourceView):
        BindableProgramArgument(shaderType, name, bindingPoint, accessType, valueType), size(size), variables(variables) {}
        
        virtual ResourceType GetResourceType() override { return ResourceType::Buffer; }
        
        const size_t& GetSize() const { return size; }
        const std::vector<ConstantBufferVariableProgramArgument>& GetVariables() const { return variables; }
    private:
        size_t size;
        std::vector<ConstantBufferVariableProgramArgument> variables;
    };

    class ProgramSettings {
    public:
        ProgramSettings() = default;
        ProgramSettings(const std::vector<ProgramInputBuffer>& layout,
                        const std::vector<Ref<Shader>>& shaders,
                        const std::map<std::string, Ref<BindableProgramArgument>>& programArguments):
        layout(layout), shaders(shaders), programArguments(programArguments) {}
        
        const std::vector<ProgramInputBuffer>& GetLayout() const { return layout; }
        const std::vector<Ref<Shader>>& GetShaders() const { return shaders; }
        const std::map<std::string, Ref<BindableProgramArgument>>& GetProgramArguments() const { return programArguments; }
    private:
        std::vector<ProgramInputBuffer> layout;
        std::vector<Ref<Shader>> shaders;
        std::map<std::string, Ref<BindableProgramArgument>> programArguments;
    };

    class Program: public Object {
    public:
        Program(Weak<Context> context, const ProgramSettings& settings): context(context), settings(settings) {}
        virtual ~Program() {};
        
        const Weak<Context> GetContext() const { return context; }
        const ProgramSettings& GetSettings() const { return settings; }
    protected:
    private:
        Weak<Context> context;
        ProgramSettings settings;
    };

    using ProgramArgumentBindingValue = std::variant<ResourceView, TextureView, int>;

    class ProgramArgumentBinding {
    public:
        ProgramArgumentBinding(const Ref<BindableProgramArgument> argument, const ProgramArgumentBindingValue& value):
        argument(argument), value(value) {}
        
        const Ref<BindableProgramArgument> GetArgument() const { return argument; }
        const ProgramArgumentBindingValue& GetValue() const { return value; }
    private:
        const Ref<BindableProgramArgument> argument;
        const ProgramArgumentBindingValue& value;
    };

    class ProgramArgumentBindings {
    public:
        ProgramArgumentBindings() = default;
        ProgramArgumentBindings(Ref<Program> program,
                                const std::unordered_map<Ref<BindableProgramArgument>, ProgramArgumentBindingValue, BindableProgramArgument::Hash>& parameters):
        program(program), parameters(parameters) {}
        
        const Ref<Program> GetProgram() const { return program; }
        const std::unordered_map<Ref<BindableProgramArgument>, ProgramArgumentBindingValue, BindableProgramArgument::Hash>& GetParameters() const {
            return parameters;
        }
        
//        const ProgramArgumentBinding Get(const BindableProgramArgument& argument) const;
//        const Ref<BindableProgramArgument>& GetArguments() const;s
        
    private:
        Ref<Program> program;
        std::unordered_map<Ref<BindableProgramArgument>, ProgramArgumentBindingValue, BindableProgramArgument::Hash> parameters;
    };
}
