//
//  HLSLOpenGLShader.cpp
//  VWolf
//
//  Created by Enrique Moises on 4/13/26.
//

#include "vwpch.h"
#include "HLSLOpenGLShader.h"
#include "VWolf/Platform/DXILHelpers.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {

    HLSLOpenGLShader::HLSLOpenGLShader(Shader& coreShader): PShader(coreShader) {
        CompileShaders(coreShader);
        CompileProgram();
        CompileRenderState(coreShader);
        
        loadFromNewShader = true;
        settings = coreShader.GetSettings();
    }

    HLSLOpenGLShader::~HLSLOpenGLShader() {
        
    }

    void HLSLOpenGLShader::CompileShaders(Shader& coreShader) {
        for(Stage stage: coreShader.GetSubShader().GetStages()) {
            RHI::ShaderSettings shaderSettings(
                                               RHI::ShaderEntryFunction(stage.GetFunctionName()),
                                               coreShader.GetName(),
                                               coreShader.GetSubShader().GetCode()
                                               );
            
            shaders.push_back(
                              DynamicAs<RHI::OpenGLShader>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateShader((RHI::ShaderType)(uint32_t)stage.GetStageType(), shaderSettings))
                              );
        }
    }

    void HLSLOpenGLShader::CompileProgram() {
        VWOLF_CORE_ASSERT(shaders.size() > 0, "The asset doesn't have shaders in it");
        
        std::vector<RHI::ProgramInputBuffer> layout;
        std::map<std::string, Ref<RHI::BindableProgramArgument>> programArguments;
        
        for(const Ref<RHI::OpenGLShader>& shader: shaders) {
            for(DXIL::Attribute& attribute: shader->GetDXILCompiledShaderWithAttributes()->GetStageInAttributes()) {
                layout.push_back(RHI::ProgramInputBuffer(
                                                         attribute.GetName(),
                                                         attribute.GetIndex(),
                                                         VWolf::DXIL::GetFormat(attribute.GetFormat()),
                                                         attribute.GetOffset(),
                                                         attribute.GetNumberOfElements(),
                                                         attribute.GetElementType()
                                                         ));
            }
            
            for (DXIL::Sampler& sampler: shader->GetDXILCompiledShaderWithAttributes()->GetSamplers()) {
                auto it = programArguments.find(sampler.GetName());
                if (it == programArguments.end()) {
                    programArguments[sampler.GetName()] =
                    CreateRef<RHI::SamplerProgramArgument>((RHI::ShaderType)(uint32_t)shader->GetType(),
                                                          sampler.GetName(),
                                                          sampler.GetBindingIndex(),
                                                          sampler.GetNumSamples());
                }
            }
            
            for (DXIL::Texture& texture: shader->GetDXILCompiledShaderWithAttributes()->GetTextures()) {
                auto it = programArguments.find(texture.GetName());
                if (it == programArguments.end()) {
                    programArguments[texture.GetName()] =
                    CreateRef<RHI::TextureProgramArgument>((RHI::ShaderType)(uint32_t)shader->GetType(),
                                                           texture.GetName(),
                                                           texture.GetBindingIndex(),
                                                           DXIL::GetTextureDimentionType(texture.GetDimension()));
                }
            }
            
            for (DXIL::ConstantBuffer& buffer: shader->GetDXILCompiledShaderWithAttributes()->GetConstantBuffers()) {
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
        program = DynamicAs<RHI::OpenGLProgram>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateProgram(programSettings));
    }

    void HLSLOpenGLShader::CompileRenderState(Shader& coreShader) {
        RHI::RenderTargetSettings renderTargetSettings = RHI::RenderTargetSettings::FromBlend(coreShader.GetSettings().GetBlend());
        
        RHI::RenderStateSettings renderStateSettings = RHI::RenderStateSettings(program,
                                                                                OpenGLDriver::GetCurrentDriver()->GetMainRenderPattern()->weak_from_this(),
                                                                                RHI::RasterizerSettings::FromRasterization(coreShader.GetSettings().GetRasterization()),
                                                                                RHI::BlendSettings(renderTargetSettings),
                                                                                RHI::DepthSettings::FromDepthStencil(coreShader.GetSettings().GetDepthStencil()));
        state = DynamicAs<RHI::OpenGLRenderState>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateRenderState(renderStateSettings));
    }

    void HLSLOpenGLShader::Bind() {
        program->Bind();
        // Configuration
        state->SetConfiguration();
    }

    void HLSLOpenGLShader::Unbind() {
        program->Unbind();
    }

    std::vector<Ref<ShaderInput>> HLSLOpenGLShader::GetMaterialInputs() const {
        std::vector<Ref<ShaderInput>> inputs;
        std::string cbMaterialName = std::string("type_cbPer") + std::string(materialName);
        const char * _materialName = cbMaterialName.c_str();
        auto lambda = [_materialName](std::pair<std::string, Ref<RHI::OpenGLUniformBuffer>> it) {
            return strcmp(it.first.c_str(), _materialName) == 0;
        };
        auto container = program->GetUniformBuffers();
        auto mat = std::find_if(container.begin(),
                                container.end(),
                                lambda);
        if (mat == container.end()) return inputs;

        Ref<RHI::OpenGLUniformBuffer> material = mat->second;
        
        if (material == nullptr) {
            return inputs;
        }
        for (std::pair<std::string, Ref<RHI::OpenGLUniform>> uniform: material->GetUniforms()) {
            if (uniform.second->GetShaderDataType() == ShaderDataType::None) continue;
            inputs.push_back(CreateRef<ShaderInput>(uniform.second->GetName().substr(cbMaterialName.length() + 1),
                                                    uniform.second->GetShaderDataType(),
                                                    uniform.second->GetIndex(),
                                                    ShaderDataTypeSize(uniform.second->GetShaderDataType()),
                                                    uniform.second->GetOffset()));
        }
        
        return inputs;
    }

    size_t HLSLOpenGLShader::GetMaterialSize() const {
        std::string cbMaterialName = std::string("type_cbPer") + std::string(materialName);
        const char * _materialName = cbMaterialName.c_str();
        auto lambda = [_materialName](std::pair<std::string, Ref<RHI::OpenGLUniformBuffer>> it) {
            return strcmp(it.first.c_str(), _materialName) == 0;
        };
        auto container = program->GetUniformBuffers();
        auto mat = std::find_if(container.begin(),
                                container.end(),
                                lambda);
        if (mat == container.end()) return 0;
        return std::find_if(container.begin(),
                            container.end(),
                            lambda)->second->GetSize();
    }

    std::vector<ShaderInput> HLSLOpenGLShader::GetTextureInputs() const {
        std::vector<ShaderInput> inputs;
        
        for (auto uBuffer: program->GetUniformBuffers()) {
            for (auto uniform: uBuffer.second->GetUniforms()) {
                if (!uniform.second->IsTexture()) continue;
                inputs.push_back(ShaderInput(uniform.second->GetName(),
                                             uniform.second->GetShaderDataType(),
                                             uniform.second->GetIndex(),
                                             uniform.second->GetTextureSize(),
                                             uniform.second->GetOffset()));
            }
        }

        for (auto uniform: program->GetDefaultUniforms()) {
            if (!uniform.second->IsTexture()) continue;
            inputs.push_back(ShaderInput(uniform.second->GetName(),
                                         uniform.second->GetShaderDataType(),
                                         uniform.second->GetIndex(),
                                         uniform.second->GetTextureSize(),
                                         uniform.second->GetOffset()));
        }
        
        return inputs;
    }

    std::string HLSLOpenGLShader::GetName() const {
        return this->m_name;
    }

    std::vector<AttributeDescriptor> HLSLOpenGLShader::GetAttributes() const {
        std::vector<AttributeDescriptor> elements(program->GetAttributes().size());
        
        for(auto& [key, value]: program->GetAttributes()) {
            AttributeDescriptor descriptor;
            descriptor.name = value.GetName();
            descriptor.index = value.GetIndex();
            descriptor.attribute = GetAttributeFromName(value.GetName());
            descriptor.dimension = value.GetNumberOfElements();
            descriptor.format = value.GetAttributeFormat();
            
            elements[value.GetIndex()] = descriptor;
        }
        
        return elements;
    }
}
