//
//  OpenGLProgram.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/11/26.
//

#include "vwpch.h"

#include "VWolf/Core/Time.h"

#include "OpenGLProgram.h"
#include "OpenGLShader.h"

namespace VWolf::RHI {

    OpenGLAttribute::OpenGLAttribute(GLuint programId, GLint index) {
        GLint length = 255;
        GLchar* nameHolder = new GLchar[length];
        GLThrowIfFailed(glGetActiveAttrib(programId, index, length, 0, &size, &type, nameHolder));
        this->index = GLThrowIfFailed(glGetAttribLocation(programId, nameHolder));
        name = std::string(nameHolder);
        delete[] nameHolder;
    }

    ShaderDataType OpenGLAttribute::GetShaderDataType() {
        switch (type) {
            case GL_FLOAT: return ShaderDataType::Float;
            case GL_FLOAT_VEC2: return ShaderDataType::Float2;
            case GL_FLOAT_VEC3: return ShaderDataType::Float3;
            case GL_FLOAT_VEC4: return ShaderDataType::Float4;
            case GL_FLOAT_MAT3: return ShaderDataType::Mat3;
            case GL_FLOAT_MAT4: return ShaderDataType::Mat4;
            case GL_INT: return ShaderDataType::Int;
            case GL_INT_VEC2: return ShaderDataType::Int2;
            case GL_INT_VEC3: return ShaderDataType::Int3;
            case GL_INT_VEC4: return ShaderDataType::Int4;
            case GL_BOOL: return ShaderDataType::Bool;
        }
        return ShaderDataType::None;
    }

    AttributeFormat OpenGLAttribute::GetAttributeFormat() {
        switch (type) {
            case GL_FLOAT: return AttributeFormat::Float32;
            case GL_FLOAT_VEC2: return AttributeFormat::Float32;
            case GL_FLOAT_VEC3: return AttributeFormat::Float32;
            case GL_FLOAT_VEC4: return AttributeFormat::Float32;
            case GL_FLOAT_MAT3: return AttributeFormat::Float32;
            case GL_FLOAT_MAT4: return AttributeFormat::Float32;
            case GL_INT: return AttributeFormat::SInt32;
            case GL_INT_VEC2: return AttributeFormat::SInt32;
            case GL_INT_VEC3: return AttributeFormat::SInt32;
            case GL_INT_VEC4: return AttributeFormat::SInt32;
            case GL_BOOL: return AttributeFormat::UInt8;
        }
        return AttributeFormat::Unknown;
    }

    uint32_t OpenGLAttribute::GetDimension() {
        switch (type) {
            case GL_FLOAT: return 1;
            case GL_FLOAT_VEC2: return 2;
            case GL_FLOAT_VEC3: return 3;
            case GL_FLOAT_VEC4: return 4;
            case GL_FLOAT_MAT3: return 3 * 3;
            case GL_FLOAT_MAT4: return 4 * 4;
            case GL_INT: return 1;
            case GL_INT_VEC2: return 2;
            case GL_INT_VEC3: return 3;
            case GL_INT_VEC4: return 4;
            case GL_BOOL: return 1;
            default: return -1;
        }
    }

    OpenGLUniform::OpenGLUniform(GLuint programId, GLint index): index(index) {
        GLint length;
        GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length));
        char* nameHolder = new char[length];
        GLThrowIfFailed(glGetActiveUniformName(programId, index, length, 0, nameHolder));
        name = std::string(nameHolder);
        delete[] nameHolder;
    }

    ShaderDataType OpenGLUniform::GetShaderDataType() {
        switch (type) {
            case GL_FLOAT: return ShaderDataType::Float;
            case GL_FLOAT_VEC2: return ShaderDataType::Float2;
            case GL_FLOAT_VEC3: return ShaderDataType::Float3;
            case GL_FLOAT_VEC4: return ShaderDataType::Float4;
            case GL_FLOAT_MAT3: return ShaderDataType::Mat3;
            case GL_FLOAT_MAT4: return ShaderDataType::Mat4;
            case GL_INT: return ShaderDataType::Int;
            case GL_INT_VEC2: return ShaderDataType::Int2;
            case GL_INT_VEC3: return ShaderDataType::Int3;
            case GL_INT_VEC4: return ShaderDataType::Int4;
            case GL_BOOL: return ShaderDataType::Bool;
        }
        return ShaderDataType::None;
    }

    bool OpenGLUniform::IsTexture() {
        switch (type) {
            case GL_SAMPLER_2D: return true;
            case GL_SAMPLER_CUBE: return true;
        }
        return false;
    }

    int OpenGLUniform::GetTextureSize() {
        if (!IsTexture()) return 1;
        switch (type) {
            case GL_SAMPLER_2D: return (int)ShaderSamplerType::Sampler2D;
            case GL_SAMPLER_CUBE: return (int)ShaderSamplerType::SamplerCube;
            default: return 2;
        }
    }
    
    int OpenGLUniformBuffer::totalBindings = 0;

    OpenGLUniformBuffer::OpenGLUniformBuffer(GLuint programId, GLint index): index(index) {
        Build(programId);
    }

    OpenGLUniformBuffer::~OpenGLUniformBuffer() {

    }

    void OpenGLUniformBuffer::AddTotalBindings(int bindings) {
        totalBindings += bindings;
    }

    void OpenGLUniformBuffer::Build(GLuint programId) {
        // Getting the name
        GLint length;
        GLThrowIfFailed(glGetActiveUniformBlockiv(programId, index, GL_UNIFORM_BLOCK_NAME_LENGTH, &length));
        char* nameHolder = new char[length];
        GLThrowIfFailed(glGetActiveUniformBlockName(programId, index, length, 0, nameHolder));
        name = std::string(nameHolder);
        delete[] nameHolder;

        // Getting the size of the block
        GLThrowIfFailed(glGetActiveUniformBlockiv(programId, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size));

        // Binding the UB
        GLThrowIfFailed(glUniformBlockBinding(programId, index, index + totalBindings));

        GLThrowIfFailed(glGetActiveUniformBlockiv(programId, index, GL_UNIFORM_BLOCK_BINDING, &binding));

        RetrieveUniforms(programId);
    }

    void OpenGLUniformBuffer::RetrieveUniforms(GLuint programId) {
        GLint ubActiveUniforms;
        GLThrowIfFailed(glGetActiveUniformBlockiv(programId, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &ubActiveUniforms));
        GLint* ubActiveUniformsIndices = new GLint[ubActiveUniforms];
        std::vector<GLuint> ubActiveUniformsIndicesNotNegative;
        GLint* ubActiveUniformsOffsets = new GLint[ubActiveUniforms];
        GLint* ubActiveUniformsTypes = new GLint[ubActiveUniforms];
        GLThrowIfFailed(glGetActiveUniformBlockiv(programId, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, ubActiveUniformsIndices));
        for(int uIndex = 0; uIndex < ubActiveUniforms; uIndex++) {
            if (ubActiveUniformsIndices[uIndex] > -1)
                ubActiveUniformsIndicesNotNegative.push_back(ubActiveUniformsIndices[uIndex]);
        }
        GLThrowIfFailed(glGetActiveUniformsiv(programId, ubActiveUniforms, ubActiveUniformsIndicesNotNegative.data(), GL_UNIFORM_TYPE, ubActiveUniformsTypes));
        GLThrowIfFailed(glGetActiveUniformsiv(programId, ubActiveUniforms, ubActiveUniformsIndicesNotNegative.data(), GL_UNIFORM_OFFSET, ubActiveUniformsOffsets));
        
        for(int uIndex = 0; uIndex < ubActiveUniforms; uIndex++) {
            Ref<OpenGLUniform> uniform = CreateRef<OpenGLUniform>(programId, ubActiveUniformsIndices[uIndex]);
            uniform->SetType(ubActiveUniformsTypes[uIndex]);
            uniform->SetOffset(ubActiveUniformsOffsets[uIndex]);
            uniforms.insert(std::pair<std::string, Ref<OpenGLUniform>>(uniform->GetName(), uniform));
        }
        delete[] ubActiveUniformsIndices;
        delete[] ubActiveUniformsOffsets;
        delete[] ubActiveUniformsTypes;
    }

    void OpenGLUniformBuffer::ReplaceProgramBinding(const Ref<BindableProgramArgument> argument, const ProgramArgumentBindingValue& value) {        
        const ResourceView resourceView = std::get<ResourceView>(value);
        Ref<OpenGLBuffer> buffer = DynamicAs<OpenGLBuffer>(resourceView.GetResource());
        
        /*if (Time::GetTotalFrames() > 1) {
            std::cout << "Buffer id: " << buffer->GetBufferId() << std::endl;
        }*/
        
        GLThrowIfFailed(glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer->GetBufferId()));
    }

    OpenGLVertexArray::OpenGLVertexArray(std::map<std::string, DXIL::Attribute> attributes, Ref<OpenGLBufferSet> vertexSet, Ref<OpenGLBuffer> indexBuffer):
    attributes(attributes), vertexSet(vertexSet), indexBuffer(indexBuffer) {
        GLThrowIfFailed(glGenVertexArrays(1, &vertexArrayId));
		indexBuffer->AttachIndexBufferToVertexArray();
        Build();
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        GLThrowIfFailed(glDeleteVertexArrays(1, &vertexArrayId));
    }

    void OpenGLVertexArray::Bind() {
        GLThrowIfFailed(glBindVertexArray(vertexArrayId));
    }

    void OpenGLVertexArray::Unbind() {
        GLThrowIfFailed(glBindVertexArray(0));
    }

    static GLenum AttributeFormatToOpenGLBaseType(RHI::PixelFormat type)
    {
        switch (type)
        {
            case RHI::PixelFormat::RGBA32Float:
                return GL_FLOAT;
            case RHI::PixelFormat::R8Sint:
                return GL_BYTE;
            case RHI::PixelFormat::R16Sint:
                return GL_SHORT;
            case RHI::PixelFormat::R32Sint:
                return GL_INT;
            case RHI::PixelFormat::R8Uint:
                return GL_UNSIGNED_BYTE;
            case RHI::PixelFormat::R16Uint:
                return GL_UNSIGNED_SHORT;
            case RHI::PixelFormat::R32Uint:
                return GL_UNSIGNED_INT;
            default: return GL_INVALID_VALUE;
        }

        VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    void OpenGLVertexArray::Build() {
        Bind();
        vertexSet->Bind();
        
        size_t offset = 0, stride = 0;
        
        for (auto& [key, element] : attributes) {
            size_t size = 0;
            switch (element.GetComponentType()) {
                case RHI::PixelFormat::RGBA32Float:
                    size = sizeof(float);
                    break;
                case RHI::PixelFormat::R8Sint:
                    size = sizeof(bool);
                    break;
                default: size = 1;
            }
            stride += (size * element.GetNumberOfElements());
        }

        for (auto& [key, element] : attributes)
        {
            switch (element.GetComponentType()) {
                
                case RHI::PixelFormat::RGBA32Float:
                    GLThrowIfFailed(glEnableVertexAttribArray(element.GetIndex()));
                    GLThrowIfFailed(glVertexAttribPointer(element.GetIndex(),
                                                          element.GetNumberOfElements(),
                                                          AttributeFormatToOpenGLBaseType(element.GetComponentType()),
                                                          GL_FALSE,
                                                          stride,
                                                          (const void*)element.GetOffset()));
                    break;
                case RHI::PixelFormat::R8Sint:
                    GLThrowIfFailed(glEnableVertexAttribArray(element.GetIndex()));
                    GLThrowIfFailed(glVertexAttribIPointer(element.GetIndex(),
                                                           element.GetNumberOfElements(),
                                                           AttributeFormatToOpenGLBaseType(element.GetComponentType()),
                                                           stride,
                                                           (const void*)element.GetOffset()));
                    break;
                default: break;
            }
        }
        vertexSet->Unbind();
        Unbind();
    }

    OpenGLProgram::OpenGLProgram(Weak<Context> context, const ProgramSettings& settings):
    Program(context, settings), programId(glCreateProgram())  {
        GLThrowIfFailedNoAction("glCreateProgram");
        
        Initialize();
    }

    void OpenGLProgram::Bind() {
        GLThrowIfFailed(glUseProgram(programId));
    }

    void OpenGLProgram::Unbind() {
        GLThrowIfFailed(glUseProgram(0));
    }

    OpenGLProgram::~OpenGLProgram() {
        GLThrowIfFailed(glDeleteProgram(programId));
    }

    void OpenGLProgram::Initialize() {
        for(Ref<Shader> shader: GetSettings().GetShaders()) {
            Ref<OpenGLShader> openGLShader = DynamicAs<OpenGLShader>(shader);
            openGLShader->Attach(programId);
        }
        
        GLThrowIfFailed(glLinkProgram(programId));
        if (!IsLinked()) {
            PrintErrorAndAssert();
        }
        
        for(Ref<Shader> shader: GetSettings().GetShaders()) {
            Ref<OpenGLShader> openGLShader = DynamicAs<OpenGLShader>(shader);
            openGLShader->Detach(programId);
        }
        
        RetrieveUniformsBlocks();
        RetrieveUniforms();
        RetrieveAttributes();
    }

    GLint OpenGLProgram::GetStatus(GLenum enumValue) {
        GLint success;
        GLThrowIfFailed(glGetProgramiv(programId, enumValue, &success));
        return success;
    }

    void OpenGLProgram::PrintErrorAndAssert() {
        GLint length = GetStatus(GL_INFO_LOG_LENGTH);
        GLchar* infoLog = new GLchar[length];
        GLThrowIfFailed(glGetProgramInfoLog(programId, length, nullptr, infoLog));
        VWOLF_CORE_DEBUG(infoLog);
        VWOLF_CORE_ASSERT(false, infoLog);
        delete[] infoLog;
    }

    void OpenGLProgram::RetrieveUniformsBlocks() {
        GLint ubCount;
        GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_UNIFORM_BLOCKS, &ubCount));
        for(int index = 0; index < ubCount; index++) {
            Ref<OpenGLUniformBuffer> ubBlock = CreateRef<OpenGLUniformBuffer>(*this, index);
            uniformBuffers.insert(std::pair<std::string, Ref<OpenGLUniformBuffer>>(ubBlock->GetName(), ubBlock));
        }
        OpenGLUniformBuffer::AddTotalBindings(ubCount);
    }

    void OpenGLProgram::RetrieveUniforms() {
        GLint uniforms;
        GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &uniforms));
        std::vector<GLuint> ubActiveUniformsIndicesNotNegative;
        for(int uIndex = 0; uIndex < uniforms; uIndex++) {
            ubActiveUniformsIndicesNotNegative.push_back(uIndex);
        }
        
        GLint* uActiveUniformsOffsets = new GLint[uniforms];
        GLint* uActiveUniformsTypes = new GLint[uniforms];
        GLint* uActiveUniformsBlockIndex = new GLint[uniforms];
        GLThrowIfFailed(glGetActiveUniformsiv(programId, uniforms, ubActiveUniformsIndicesNotNegative.data(), GL_UNIFORM_TYPE, uActiveUniformsTypes));
        GLThrowIfFailed(glGetActiveUniformsiv(programId, uniforms, ubActiveUniformsIndicesNotNegative.data(), GL_UNIFORM_OFFSET, uActiveUniformsOffsets));
        GLThrowIfFailed(glGetActiveUniformsiv(programId, uniforms, ubActiveUniformsIndicesNotNegative.data(), GL_UNIFORM_BLOCK_INDEX, uActiveUniformsBlockIndex));
        
        for(int uIndex = 0; uIndex < uniforms; uIndex++) {
            if (uActiveUniformsBlockIndex[uIndex] != -1) continue;
            Ref<OpenGLUniform> uniform = CreateRef<OpenGLUniform>(programId, uIndex);
            uniform->SetType(uActiveUniformsTypes[uIndex]);
            uniform->SetOffset(uActiveUniformsOffsets[uIndex]);
    #ifdef VWOLF_PLATFORM_WINDOWS
            GLuint _index = glGetUniformLocation(programId, uniform->GetName().c_str());
            if (uIndex != _index)
                uniform->SetIndex(_index);
    #endif
            defaultUniforms.insert(std::pair<std::string, Ref<OpenGLUniform>>(uniform->GetName(), uniform));
        }
        delete[] uActiveUniformsOffsets;
        delete[] uActiveUniformsTypes;
    }

    void OpenGLProgram::RetrieveAttributes() {
        for(Ref<Shader> shader: GetSettings().GetShaders()) {
            Ref<OpenGLShader> openGLShader = DynamicAs<OpenGLShader>(shader);
            for (DXIL::Attribute dxilAttribute : openGLShader->GetDXILCompiledShaderWithAttributes()->GetStageInAttributes())
                dxilAttributes[dxilAttribute.GetName()] = dxilAttribute;
            GLint attributesCount;
            GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &attributesCount));
            for(int aIndex = 0; aIndex < attributesCount; aIndex++) {
                Ref<OpenGLAttribute> attribute = CreateRef<OpenGLAttribute>(programId, aIndex);
                attributes[attribute->GetName()] = attribute;
            }
        }
    }

    void OpenGLProgram::ReplaceProgramBinding(const Ref<BindableProgramArgument> argument, const ProgramArgumentBindingValue& value) {
        Ref<ConstantBufferProgramArgument> constantBufferArgument = DynamicAs<ConstantBufferProgramArgument>(argument);
        if (constantBufferArgument == nullptr) return;
        
        auto lambda = [constantBufferArgument](std::pair<std::string, Ref<OpenGLUniformBuffer>> it) {
            std::string name = it.first;
            boost::algorithm::replace_all(name, "type_", "");
            return name == constantBufferArgument->GetName();
        };
        
        auto ubPair = std::find_if(uniformBuffers.begin(), uniformBuffers.end(), lambda);
        if (ubPair == uniformBuffers.end()) return;
        if (ubPair->first == "") return;
        
        Ref<OpenGLUniformBuffer> ubBlock = ubPair->second;
        ubBlock->ReplaceProgramBinding(argument, value);
    }

    Ref<OpenGLVertexArray> OpenGLProgram::CreateVertexArray(Ref<OpenGLBufferSet> vertexSet, Ref<OpenGLBuffer> indexBuffer) {
        return CreateRef<OpenGLVertexArray>(dxilAttributes, vertexSet, indexBuffer);
    }

    void OpenGLProgram::ReplaceTextureBinding(const Ref<BindableProgramArgument> argument, const GLuint binding) {
        
		Ref<OpenGLUniform> textureUniform = nullptr;

        for (auto uBuffer : this->GetUniformBuffers()) {
            for (auto uniform : uBuffer.second->GetUniforms()) {
                if (!uniform.second->IsTexture()) continue;
                
                if (uniform.first == argument->GetName()) {
                    textureUniform = uniform.second;
                    break;
                }
            }
        }

        for (auto uniform : GetDefaultUniforms()) {
            if (!uniform.second->IsTexture()) continue;
            if (uniform.first == argument->GetName()) {
                textureUniform = uniform.second;
                break;
            }
        }

		if (textureUniform == nullptr) return;

		GLThrowIfFailed(glUniform1i(textureUniform->GetIndex(), binding));
    }
}
