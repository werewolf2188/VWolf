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
#include "VWolf/Platform/Metal/Render/RHI/MetalDriver.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalTypes.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalDriver.h"

#include "VWolf/Platform/DXILHelpers.h"

// bool IRShaderReflectionCopyVertexInfo(const IRShaderReflection* reflection, IRReflectionVersion version, IRVersionedVSInfo* vsinfo);
#define IR_RUNTIME_METALCPP       // enable metal-cpp compatibility mode
#include <metal_irconverter_runtime/metal_irconverter_runtime.h>

namespace VWolf {

//    void process_data(unsigned char* data_buffer, size_t length) {
//        // Function that expects a char*
//        for (size_t i = 0; i < length; ++i) {
//            std::cout << data_buffer[i];
//        }
//        std::cout << std::endl;
//    }

    HLSLMetalShader::HLSLMetalShader(Shader& coreShader): PShader(coreShader) {
        settings = coreShader.GetSettings();
        CompileShaders(coreShader);
        CompileProgram();
        BuildState(coreShader);
    }

    void HLSLMetalShader::CompileShaders(Shader& coreShader) {

        for (Stage stage: coreShader.GetSubShader().GetStages()) {

            RHI::ShaderSettings shaderSettings(
                                                                    RHI::ShaderEntryFunction(stage.GetFunctionName()),
                                                                    coreShader.GetName(),
                                                                    coreShader.GetSubShader().GetCode()
                                                                    );
            
            shaders.push_back(
                              DynamicAs<RHI::MetalShader>(MetalDriver::GetCurrent()->GetRenderContext()->CreateShader((RHI::ShaderType)(uint32_t)stage.GetStageType(), shaderSettings))
                              );
        }
    }

    void HLSLMetalShader::CompileProgram() {
        VWOLF_CORE_ASSERT(shaders.size() > 0, "The asset doesn't have shaders in it");
        
        std::vector<RHI::ProgramInputBuffer> layout;
        std::map<std::string, Ref<RHI::BindableProgramArgument>> programArguments;
        
        for(const Ref<RHI::MetalShader>& shader: shaders) {
            for(DXIL::Attribute& attribute: shader->GetDXILCompiledShader()->GetStageInAttributes()) {
                layout.push_back(RHI::ProgramInputBuffer(
                                                         attribute.GetName(),
                                                         attribute.GetIndex(),
                                                         VWolf::DXIL::GetFormat(attribute.GetFormat()),
                                                         attribute.GetOffset(),
                                                         attribute.GetNumberOfElements(),
                                                         attribute.GetElementType()
                                                         ));
            }
            
            for (DXIL::Sampler& sampler: shader->GetDXILCompiledShader()->GetSamplers()) {
                auto it = programArguments.find(sampler.GetName());
                if (it == programArguments.end()) {
                    programArguments[sampler.GetName()] =
                    CreateRef<RHI::SamplerProgramArgument>((RHI::ShaderType)(uint32_t)shader->GetType(),
                                                          sampler.GetName(),
                                                          sampler.GetBindingIndex(),
                                                          sampler.GetNumSamples());
                }
            }
            
            for (DXIL::Texture& texture: shader->GetDXILCompiledShader()->GetTextures()) {
                auto it = programArguments.find(texture.GetName());
                if (it == programArguments.end()) {
                    programArguments[texture.GetName()] =
                    CreateRef<RHI::TextureProgramArgument>((RHI::ShaderType)(uint32_t)shader->GetType(),
                                                           texture.GetName(),
                                                           texture.GetBindingIndex(),
                                                           DXIL::GetTextureDimentionType(texture.GetDimension()));
                }
            }
            
            for (DXIL::ConstantBuffer& buffer: shader->GetDXILCompiledShader()->GetConstantBuffers()) {
                auto it = programArguments.find(buffer.GetName());
                if (it == programArguments.end()) {
                    std::vector<RHI::ConstantBufferVariableProgramArgument> variables;
                    for (DXIL::ConstantBufferVariable& variable: buffer.GetVariables()) {
                        variables.push_back(RHI::ConstantBufferVariableProgramArgument(
                                                                                       (RHI::ShaderType)(uint32_t)shader->GetType(),
                                                                                       variable.GetName(),
                                                                                       variable.GetSize(),
                                                                                       variable.GetOffset(),
                                                                                       variable.GetIndex(),
                                                                                       (RHI::DataType)(uint32_t)variable.GetShaderDataType()
                                                                                       ));
                    }
                    programArguments[buffer.GetName()] =
                    CreateRef<RHI::ConstantBufferProgramArgument>((RHI::ShaderType)(uint32_t)shader->GetType(),
                                                                  buffer.GetName(),
                                                                  buffer.GetBindingIndex(),
                                                                  buffer.GetSize(),
                                                                  variables);
                }
            }
        }
        
        RHI::ProgramSettings programSettings = RHI::ProgramSettings(layout, std::vector<Ref<RHI::Shader>>(shaders.begin(), shaders.end()), programArguments);
        program = DynamicAs<RHI::MetalProgram>(MetalDriver::GetCurrent()->GetRenderContext()->CreateProgram(programSettings));
    }

    void HLSLMetalShader::BuildState(Shader& coreShader) {
        
        RHI::RenderTargetSettings renderTargetSettings = RHI::RenderTargetSettings::FromBlend(coreShader.GetSettings().GetBlend());
        
        RHI::RenderStateSettings renderStateSettings = RHI::RenderStateSettings(program,
                                                                                MetalDriver::GetCurrent()->GetMainRenderPattern()->weak_from_this(),
                                                                                RHI::RasterizerSettings::FromRasterization(coreShader.GetSettings().GetRasterization()),
                                                                                RHI::BlendSettings(renderTargetSettings),
                                                                                RHI::DepthSettings::FromDepthStencil(coreShader.GetSettings().GetDepthStencil()));
        state = DynamicAs<RHI::MetalRenderState>(MetalDriver::GetCurrent()->GetRenderContext()->CreateRenderState(renderStateSettings));
    }

    HLSLMetalShader::~HLSLMetalShader() {
        
    }

    std::string HLSLMetalShader::GetName() const {
        return this->m_name;
    }

    std::vector<Ref<ShaderInput>> HLSLMetalShader::GetMaterialInputs() const {
        std::string cbMaterialName = std::string("cbPer") + std::string(materialName);
        
        std::map<std::string, Ref<RHI::BindableProgramArgument>> programArguments;
        std::copy_if(program->GetSettings().GetProgramArguments().begin(), program->GetSettings().GetProgramArguments().end(),
                         std::inserter(programArguments, programArguments.end()),
                         [](const auto& pair) {
                             return pair.second->GetResourceType() == RHI::ResourceType::Buffer;
                         });
        
        if (programArguments.count(cbMaterialName) == 0) return {};
        
        Ref<RHI::ConstantBufferProgramArgument> material = DynamicAs<RHI::ConstantBufferProgramArgument>(programArguments[cbMaterialName]);
        
        std::vector<Ref<ShaderInput>> inputs;

        for (RHI::ConstantBufferVariableProgramArgument variable : material->GetVariables()) {
            if (variable.GetDataType() == RHI::DataType::None) continue;
            inputs.push_back(CreateRef<ShaderInput>(variable.GetName(),
                                                    (ShaderDataType)(uint32_t)variable.GetDataType(),
                                                    variable.GetIndex(),
                                                    ShaderDataTypeSize((ShaderDataType)(uint32_t)variable.GetDataType()),
                                                    variable.GetOffset()));
        }
        return inputs;
    }

    size_t HLSLMetalShader::GetMaterialSize() const {
        std::string cbMaterialName = std::string("cbPer") + std::string(materialName);
        
        std::map<std::string, Ref<RHI::BindableProgramArgument>> programArguments;
        std::copy_if(program->GetSettings().GetProgramArguments().begin(), program->GetSettings().GetProgramArguments().end(),
                         std::inserter(programArguments, programArguments.end()),
                         [](const auto& pair) {
                             return pair.second->GetResourceType() == RHI::ResourceType::Buffer;
                         });
        
        if (programArguments.count(cbMaterialName) != 0) {
            return DynamicAs<RHI::ConstantBufferProgramArgument>(programArguments[cbMaterialName])->GetSize();
        }
        return 0;
    }

    std::vector<ShaderInput> HLSLMetalShader::GetTextureInputs() const {
        std::vector<ShaderInput> inputs;
        
        std::map<std::string, Ref<RHI::BindableProgramArgument>> programArguments;
        std::copy_if(program->GetSettings().GetProgramArguments().begin(), program->GetSettings().GetProgramArguments().end(),
                         std::inserter(programArguments, programArguments.end()),
                         [](const auto& pair) {
                             return pair.second->GetResourceType() == RHI::ResourceType::Texture;
                         });

        for (auto [key, variable]: programArguments) {
            Ref<RHI::TextureProgramArgument> texture = DynamicAs<RHI::TextureProgramArgument>(variable);
            inputs.push_back(ShaderInput(texture->GetName(),
                                         ShaderDataType::Float4,
                                         (uint32_t)texture->GetBindingPoint(),
                                         (uint32_t)texture->GetTextureDimensionType(),
                                         0));
        }
        return inputs;
    }

    std::vector<AttributeDescriptor> HLSLMetalShader::GetAttributes() const {
        std::vector<AttributeDescriptor> elements(program->GetStageInAttributes().size());
       
        for(const Ref<RHI::StageInAttribute>& value:  program->GetStageInAttributes()) {
            AttributeDescriptor descriptor;
            descriptor.name = value->GetName();
            descriptor.index = (int32_t)value->GetIndex();
            descriptor.attribute = GetAttributeFromName(value->GetName());
            descriptor.dimension = value->GetDimension();
            descriptor.format = value->GetAttributeFormat();
            
            elements[value->GetIndex()] = descriptor;
        }
        
        return elements;
    }
}

#endif
