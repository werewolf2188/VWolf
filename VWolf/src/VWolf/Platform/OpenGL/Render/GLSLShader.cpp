#include "vwpch.h"
#include "GLSLShader.h"

#include "VWolf/Core/Utils/File.h"

#include <glm/gtc/type_ptr.hpp>

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

#define MESSAGE_LENGTH 512

namespace VWolf {
    class GLShaderSource {
    public:
        GLShaderSource(ShaderSource source, bool compile = true) {
            switch (source.sourceType) {
                case ShaderSourceType::File:
                    sourceText = File::OpenTextFile(source.shader.c_str());
                    break;
                case ShaderSourceType::Text:
                    sourceText = source.shader;
                    break;
                case ShaderSourceType::Binary:
                    VWOLF_CORE_ASSERT(false, "Shaders cannot be binary at the moment");
                    break;
            }
            type = ShaderTypeEquivalent(source.type);
            shaderId = glCreateShader(type);
            GLThrowIfFailedNoAction("glCreateShader");
            if (compile)
                Compile();
        }
        ~GLShaderSource() {
            if (!HasBeenDeleted())
                GLThrowIfFailed(glDeleteShader(shaderId));
        }
    
        bool HasBeenDeleted() {
            return GetStatus(GL_DELETE_STATUS) == GL_TRUE;
        }

        bool IsCompiled() {
            return GetStatus(GL_COMPILE_STATUS) == GL_TRUE;
        }

        void Compile() {
            const char* shaderText = sourceText.c_str();
            Compile(shaderText);
        }

        void Delete() {
            GLThrowIfFailed(glDeleteShader(shaderId));
        }

        GLint GetSize() {
            return GetStatus(GL_SHADER_SOURCE_LENGTH);
        }

        bool IsAttachedToProgram() {
            return isAttached;
        }

        friend class GLProgram;
    private:
        GLint GetStatus(GLenum enumValue) {
            GLint success;
            GLThrowIfFailed(glGetShaderiv(shaderId, enumValue, &success));
            return success;
        }

        void Compile(const char* shaderText) {
            GLThrowIfFailed(glShaderSource(shaderId, 1, &shaderText, NULL));
            GLThrowIfFailed(glCompileShader(shaderId));
            if (!IsCompiled()) {
                PrintError();
                VWOLF_CORE_ASSERT(false, "Something happened");
            }
        }

        void PrintError() {
            GLint length = GetStatus(GL_INFO_LOG_LENGTH);
            GLchar* infoLog = new GLchar[length];
            GLThrowIfFailed(glGetShaderInfoLog(shaderId, length, nullptr, infoLog));
            VWOLF_CORE_DEBUG(infoLog);
            delete[] infoLog;
        }

        GLuint ShaderTypeEquivalent(ShaderType type) {
            switch (type) {
                case ShaderType::Vertex: return GL_VERTEX_SHADER;
                case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
                case ShaderType::Pre_Tesselator: return GL_TESS_CONTROL_SHADER;
                case ShaderType::Post_Tesselator: return GL_TESS_EVALUATION_SHADER;
                case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
                case ShaderType::Compute: return GL_COMPUTE_SHADER;
            }
        }
    private:
        GLuint shaderId;
        std::string sourceText;
        GLenum type;
        bool isAttached;
    };

    class GLAttribute {
    public:
        GLAttribute(GLuint programId, GLint index) {
            GLint length = 255;
            GLchar* nameHolder = new GLchar[length];
            GLThrowIfFailed(glGetActiveAttrib(programId, index, length, 0, &size, &type, nameHolder));
            name = std::string(nameHolder);
            delete[] nameHolder;
        }
    
        ~GLAttribute() = default;
        std::string GetName() {
            return name;
        }
        GLenum GetType() { return type; }
        GLint GetIndex() { return index; }
        GLint GetSize() { return size; }
    private:
        GLint index;
        std::string name;
        GLenum type;
        GLint size;
    };

    class GLUniform {
    public:
        GLUniform(GLuint programId, GLint index): index(index) {
            GLint length;
            GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length));
            char* nameHolder = new char[length];
            GLThrowIfFailed(glGetActiveUniformName(programId, index, length, 0, nameHolder));
            name = std::string(nameHolder);
            delete[] nameHolder;
        }

        ~GLUniform() = default;

        std::string GetName() {
            return name;
        }

        void SetIndex(GLint index) {
            this->index = index;
        }

        void SetType(GLint type) {
            this->type = type;
        }

        void SetOffset(GLint offset) {
            this->offset = offset;
        }

        GLint GetIndex() {
            return index;
        }

        GLuint GetType() {
            return type;
        }

        ShaderDataType GetShaderDataType() {
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

        bool IsTexture() {
            switch (type) {
                case GL_SAMPLER_2D: return true;
                case GL_SAMPLER_CUBE: return true;
            }
            return false;
        }

        int GetTextureSize() {
            if (!IsTexture()) return 1;
            switch (type) {
                case GL_SAMPLER_2D: return (int)ShaderSamplerType::Sampler2D;
                case GL_SAMPLER_CUBE: return (int)ShaderSamplerType::SamplerCube;
                default: return 2;
            }
        }

        GLuint GetOffset() {
            return offset;
        }
    private:
        GLint index;
        std::string name;
        GLint type;
        GLint offset;
    };

    class GLUniformBuffer {
    public:
    
        GLUniformBuffer(GLuint programId, GLint index): index(index) {
            GLThrowIfFailed(glGenBuffers(1, &id));
            Build(programId);
        }

        ~GLUniformBuffer() {
            GLThrowIfFailed(glDeleteBuffers(1, &id));
        }

        std::string GetName() {
            return name;
        }

        GLint GetIndex() {
            return index;
        }

        GLint GetBinding() {
            return binding;
        }

        GLint GetSize() {
            return size;
        }
        
        std::map<std::string, Ref<GLUniform>> GetUniforms() {
            return uniforms;
        }

        void SetData(const void* data, uint32_t size, uint32_t offset) {
            GLThrowIfFailed(glBindBuffer(GL_UNIFORM_BUFFER, id));
            GLThrowIfFailed(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
            GLThrowIfFailed(glBindBuffer(GL_UNIFORM_BUFFER, 0));
        }
    public:
        static void AddTotalBindings(int bindings) {
            totalBindings += bindings;
        }
    private:
        void Build(GLuint programId) {
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
   
            // Sizing the buffer
            GLThrowIfFailed(glBindBuffer(GL_UNIFORM_BUFFER, id));
            GLThrowIfFailed(glGetActiveUniformBlockiv(programId, index, GL_UNIFORM_BLOCK_BINDING, &binding));
            GLThrowIfFailed(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
            GLThrowIfFailed(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    
            // Setting the base
            GLThrowIfFailed(glBindBufferBase(GL_UNIFORM_BUFFER, binding, id));
            RetrieveUniforms(programId);
        }

        void RetrieveUniforms(GLuint programId) {
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
                Ref<GLUniform> uniform = CreateRef<GLUniform>(programId, ubActiveUniformsIndices[uIndex]);
                uniform->SetType(ubActiveUniformsTypes[uIndex]);
                uniform->SetOffset(ubActiveUniformsOffsets[uIndex]);
                uniforms.insert(std::pair<std::string, Ref<GLUniform>>(uniform->GetName(), uniform));
            }
            delete[] ubActiveUniformsIndices;
            delete[] ubActiveUniformsOffsets;
            delete[] ubActiveUniformsTypes;
        }
    private:
        GLuint id;
        GLint binding;
        GLint size;
        GLint index;
        std::string name;
        std::map<std::string, Ref<GLUniform>> uniforms;
    private:
        static int totalBindings;
    };

    int GLUniformBuffer::totalBindings = 0;

    class GLProgram {
    public:
        GLProgram(): programId(glCreateProgram()) {
            GLThrowIfFailedNoAction("glCreateProgram");
        }
        ~GLProgram() {
            Clear();
            GLThrowIfFailed(glDeleteProgram(programId));
        }

        void AddShader(Ref<GLShaderSource> source) {
            sources.push_back(source);
        }

        void RemoveShader(Ref<GLShaderSource> source) {
            auto lambda = [source](Ref<GLShaderSource> it) {
                return it->shaderId == source->shaderId;
            };
            Ref<GLShaderSource> found = *std::remove_if(sources.begin(), sources.end(), lambda);
            DettachShader(found);
        }

        void Clear() {
            DettachShaders();
            sources.clear();
        }

        void AttachShader(Ref<GLShaderSource> source) {
            AddShader(source);
            AttachShaderWithoutAdding(source);
        }

        void AttachShaders() {
            for (Ref<GLShaderSource> source: sources) {
                if (!source->isAttached)
                    AttachShaderWithoutAdding(source);
            }
        }

        void DettachShader(Ref<GLShaderSource> source) {
            GLThrowIfFailed(glDetachShader(programId, source->shaderId));
            source->isAttached = false;
        }

        void DettachShaders() {
            for (Ref<GLShaderSource> source: sources) {
                if (source->isAttached)
                    DettachShader(source);
            }
        }

        void Link() {
            GLThrowIfFailed(glLinkProgram(programId));
            if (!IsLinked()) {
                PrintErrorAndAssert();
            }
        }

        void Validate() {
            GLThrowIfFailed(glValidateProgram(programId));
            if (!IsValid())
                PrintErrorAndAssert();
        }

        void RetrieveUniforms() {
            GLint ubCount;
            GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_UNIFORM_BLOCKS, &ubCount));
            for(int index = 0; index < ubCount; index++) {
                Ref<GLUniformBuffer> ubBlock = CreateRef<GLUniformBuffer>(*this, index);
                uniformBuffers.insert(std::pair<std::string, Ref<GLUniformBuffer>>(ubBlock->GetName(), ubBlock));
            }
            GLUniformBuffer::AddTotalBindings(ubCount);
            RetrieveDefaultUniforms();
            RetrieveAttributes();
        }

        void SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
            Ref<GLUniformBuffer> ubBlock = 0;
            auto lambda = [name](std::pair<std::string, Ref<GLUniformBuffer>> it) {
                return strcmp(it.first.c_str(), name) == 0;
            };
            auto ubPair = std::find_if(uniformBuffers.begin(), uniformBuffers.end(), lambda);
            if (ubPair == uniformBuffers.end()) return;
            if (ubPair->first == "") return;
            ubBlock = ubPair->second;
            ubBlock->SetData(data, size, offset);
        }

        void Bind() {
            GLThrowIfFailed(glUseProgram(programId));
        }

        void Unbind() {
            GLThrowIfFailed(glUseProgram(0));
        }

        bool IsLinked() {
            return GetStatus(GL_LINK_STATUS) == GL_TRUE;
        }

        bool IsValid() {
            return GetStatus(GL_VALIDATE_STATUS) == GL_TRUE;
        }

        bool HasBeenDeleted() {
            return GetStatus(GL_DELETE_STATUS) == GL_TRUE;
        }

        GLint GetAttachedShadersCount() {
            return GetStatus(GL_ATTACHED_SHADERS);
        }

        std::vector<Ref<GLShaderSource>> GetSources() {
            return sources;
        }

        std::map<std::string, Ref<GLUniformBuffer>> GetUniformBuffers() {
            return uniformBuffers;
        }

        std::map<std::string, Ref<GLUniform>> GetDefaultUniforms() {
            return defaultUniforms;
        }

        std::map<std::string, Ref<GLAttribute>> GetAttributes() {
            return attributes;
        }

        operator GLuint() const {
            return programId;
        }
    private:
        GLint GetStatus(GLenum enumValue) {
            GLint success;
            GLThrowIfFailed(glGetProgramiv(programId, enumValue, &success));
            return success;
        }

        void PrintErrorAndAssert() {
            GLint length = GetStatus(GL_INFO_LOG_LENGTH);
            GLchar* infoLog = new GLchar[length];
            GLThrowIfFailed(glGetProgramInfoLog(programId, length, nullptr, infoLog));
            VWOLF_CORE_DEBUG(infoLog);
            VWOLF_CORE_ASSERT(false, infoLog);
            delete[] infoLog;
        }
    
        void AttachShaderWithoutAdding(Ref<GLShaderSource> source) {
            GLThrowIfFailed(glAttachShader(programId, source->shaderId));
            source->isAttached = true;
        }

        void RetrieveDefaultUniforms() {
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
                Ref<GLUniform> uniform = CreateRef<GLUniform>(programId, uIndex);
                uniform->SetType(uActiveUniformsTypes[uIndex]);
                uniform->SetOffset(uActiveUniformsOffsets[uIndex]);
#ifdef VWOLF_PLATFORM_WINDOWS
                GLuint _index = glGetUniformLocation(programId, uniform->GetName().c_str());
                if (uIndex != _index)
                    uniform->SetIndex(_index);
#endif
                defaultUniforms.insert(std::pair<std::string, Ref<GLUniform>>(uniform->GetName(), uniform));
            }
            delete[] uActiveUniformsOffsets;
            delete[] uActiveUniformsTypes;
        }

        void RetrieveAttributes() {
            GLint attributesCount;
            GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &attributesCount));
            for(int aIndex = 0; aIndex < attributesCount; aIndex++) {
                Ref<GLAttribute> attribute = CreateRef<GLAttribute>(programId, aIndex);
                attributes.insert(std::pair<std::string, Ref<GLAttribute>>(attribute->GetName(), attribute));
            }
        }
    private:
        GLuint programId;
        std::vector<Ref<GLShaderSource>> sources;
        std::map<std::string, Ref<GLUniformBuffer>> uniformBuffers;
        std::map<std::string, Ref<GLUniform>> defaultUniforms;
        std::map<std::string, Ref<GLAttribute>> attributes;
    };

    GLSLShader::GLSLShader(std::string name,
                           std::initializer_list<ShaderSource> otherShaders,
                           ShaderConfiguration configuration): Shader(name,
                                                                      otherShaders,
                                                                      configuration)
    {
        m_program = CreateRef<GLProgram>();
        for(ShaderSource source: m_otherShaders)
            m_program->AttachShader(CreateRef<GLShaderSource>(source));
        m_program->Link();
//        m_program->Validate();
        m_program->DettachShaders();
        m_program->RetrieveUniforms();
    }

	GLSLShader::~GLSLShader()
	{
	}

	void GLSLShader::Bind() const
	{
        m_program->Bind();
        // Configuration
        SetConfiguration();
	}

    void GLSLShader::SetConfiguration() const {
        SetRasterization();
        SetBlend();
        SetDepthStencil();
    }

    void GLSLShader::SetRasterization() const {
        // Rasterization
        if (m_configuration.rasterization.cullEnabled) {
            GLThrowIfFailed(glEnable(GL_CULL_FACE));
        }
        else {
            GLThrowIfFailed(glDisable(GL_CULL_FACE));
        }

        switch (m_configuration.rasterization.cullMode) {
            case ShaderConfiguration::Rasterization::CullMode::Front:
                GLThrowIfFailed(glCullFace(GL_FRONT));
                break;
            case ShaderConfiguration::Rasterization::CullMode::Back:
                GLThrowIfFailed(glCullFace(GL_BACK));
                break;
            case ShaderConfiguration::Rasterization::CullMode::FrontAndBack:
                GLThrowIfFailed(glCullFace(GL_FRONT_AND_BACK));
                break;
        }

        switch (m_configuration.rasterization.fillMode) {
            case ShaderConfiguration::Rasterization::FillMode::Wireframe:
                GLThrowIfFailed(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                break;
            case ShaderConfiguration::Rasterization::FillMode::Solid:
                GLThrowIfFailed(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                break;
        }

        GLThrowIfFailed(glFrontFace(m_configuration.rasterization.counterClockwise ? GL_CCW: GL_CW));

    }

    GLuint GetBlendFunction(ShaderConfiguration::Blend::Function function) {
        switch (function) {
            case ShaderConfiguration::Blend::Function::Zero: return GL_ZERO;
            case ShaderConfiguration::Blend::Function::One: return GL_ONE;
            case ShaderConfiguration::Blend::Function::SrcColor: return GL_SRC_COLOR;
            case ShaderConfiguration::Blend::Function::InvSrcColor: return GL_ONE_MINUS_SRC_COLOR;
            case ShaderConfiguration::Blend::Function::DstColor: return GL_DST_COLOR;
            case ShaderConfiguration::Blend::Function::InvDstColor: return GL_ONE_MINUS_DST_COLOR;
            case ShaderConfiguration::Blend::Function::SrcAlpha: return GL_SRC_ALPHA;
            case ShaderConfiguration::Blend::Function::InvSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case ShaderConfiguration::Blend::Function::DstAlpha: return GL_DST_ALPHA;
            case ShaderConfiguration::Blend::Function::InvDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
            case ShaderConfiguration::Blend::Function::Src1Color: return GL_SRC1_COLOR;
            case ShaderConfiguration::Blend::Function::InvSrc1Color: return GL_ONE_MINUS_SRC1_COLOR;
            case ShaderConfiguration::Blend::Function::Src1Alpha: return GL_SRC1_ALPHA;
            case ShaderConfiguration::Blend::Function::InvSrc1Alpha: return GL_ONE_MINUS_SRC1_ALPHA;
            case ShaderConfiguration::Blend::Function::SrcAlphaSat: return GL_SRC_ALPHA_SATURATE;
            case ShaderConfiguration::Blend::Function::CnstColor: return GL_CONSTANT_COLOR;
            case ShaderConfiguration::Blend::Function::InvCnstColor: return GL_ONE_MINUS_CONSTANT_COLOR;
            case ShaderConfiguration::Blend::Function::CnstAlpha: return GL_CONSTANT_ALPHA;
            case ShaderConfiguration::Blend::Function::InvCnstAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
        }
        return GL_ZERO;
    }

    void GLSLShader::SetBlend() const {
        // Blend
        if (m_configuration.blend.enabled) {
            GLThrowIfFailed(glEnable(GL_BLEND));
        }
        else {
            GLThrowIfFailed(glDisable(GL_BLEND));
        }

        GLuint sourceFunction, destinationFunction;
        sourceFunction = GetBlendFunction(m_configuration.blend.sourceFunction);
        destinationFunction = GetBlendFunction(m_configuration.blend.destinationFunction);
        GLThrowIfFailed(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, sourceFunction, destinationFunction));
        
        switch (m_configuration.blend.equation) {
            case ShaderConfiguration::Blend::Equation::Add:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_ADD));
                break;
            case ShaderConfiguration::Blend::Equation::Substract:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_SUBTRACT));
                break;
            case ShaderConfiguration::Blend::Equation::ReverseSubstract:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_REVERSE_SUBTRACT));
                break;
            case ShaderConfiguration::Blend::Equation::Min:
                GLThrowIfFailed(glBlendEquation(GL_MIN));
                break;
            case ShaderConfiguration::Blend::Equation::Max:
                GLThrowIfFailed(glBlendEquation(GL_MAX));
                break;
        }
    }

    void GLSLShader::SetDepthStencil() const {
        // Depth/Stencil
        if (m_configuration.depthStencil.depthTest) {
            GLThrowIfFailed(glEnable(GL_DEPTH_TEST));
        }
        else {
            GLThrowIfFailed(glDisable(GL_DEPTH_TEST));
        }

        switch(m_configuration.depthStencil.depthFunction) {
            case ShaderConfiguration::DepthStencil::DepthFunction::Never:
                GLThrowIfFailed(glDepthFunc(GL_NEVER));
                break;
            case ShaderConfiguration::DepthStencil::DepthFunction::Less:
                GLThrowIfFailed(glDepthFunc(GL_LESS));
                break;
            case ShaderConfiguration::DepthStencil::DepthFunction::LEqual:
                GLThrowIfFailed(glDepthFunc(GL_LEQUAL));
                break;
            case ShaderConfiguration::DepthStencil::DepthFunction::Equal:
                GLThrowIfFailed(glDepthFunc(GL_EQUAL));
                break;
            case ShaderConfiguration::DepthStencil::DepthFunction::NotEqual:
                GLThrowIfFailed(glDepthFunc(GL_NOTEQUAL));
                break;
            case ShaderConfiguration::DepthStencil::DepthFunction::GEqual:
                GLThrowIfFailed(glDepthFunc(GL_GEQUAL));
                break;
            case ShaderConfiguration::DepthStencil::DepthFunction::Greater:
                GLThrowIfFailed(glDepthFunc(GL_GREATER));
                break;
            case ShaderConfiguration::DepthStencil::DepthFunction::Always:
                GLThrowIfFailed(glDepthFunc(GL_ALWAYS));
                break;
        }
    }

	void GLSLShader::Unbind() const
	{
        m_program->Unbind();
	}

    std::string GLSLShader::GetName() const
	{
		return this->m_name;
	}

    void GLSLShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
        m_program->SetData(data, name, size, offset);
    }

    std::vector<Ref<ShaderInput>> GLSLShader::GetMaterialInputs() const {
        std::vector<Ref<ShaderInput>> inputs;
        const char * _materialName = materialName.c_str();
        auto lambda = [_materialName](std::pair<std::string, Ref<GLUniformBuffer>> it) {
            return strcmp(it.first.c_str(), _materialName) == 0;
        };
        auto container = m_program->GetUniformBuffers();
        auto mat = std::find_if(container.begin(),
                                container.end(),
                                lambda);
        if (mat == container.end()) return inputs;

        Ref<GLUniformBuffer> material = mat->second;       
        
        if (material == nullptr) {
            return inputs;
        }
        for (std::pair<std::string, Ref<GLUniform>> uniform: material->GetUniforms()) {
            if (uniform.second->GetShaderDataType() == ShaderDataType::None) continue;
            inputs.push_back(CreateRef<ShaderInput>(uniform.second->GetName(),
                                                    uniform.second->GetShaderDataType(),
                                                    uniform.second->GetIndex(),
                                                    ShaderDataTypeSize(uniform.second->GetShaderDataType()),
                                                    uniform.second->GetOffset()));
        }
        
        return inputs;
    }

    size_t GLSLShader::GetMaterialSize() const {
        const char * _materialName = materialName.c_str();
        auto lambda = [_materialName](std::pair<std::string, Ref<GLUniformBuffer>> it) {
            return strcmp(it.first.c_str(), _materialName) == 0;
        };
        auto container = m_program->GetUniformBuffers();
        auto mat = std::find_if(container.begin(),
                                container.end(),
                                lambda);
        if (mat == container.end()) return 0;
        return std::find_if(container.begin(),
                            container.end(),
                            lambda)->second->GetSize();
    }

    std::vector<ShaderInput> GLSLShader::GetTextureInputs() const {
        std::vector<ShaderInput> inputs;
        
        for (auto uBuffer: m_program->GetUniformBuffers()) {
            for (auto uniform: uBuffer.second->GetUniforms()) {
                if (!uniform.second->IsTexture()) continue;
                inputs.push_back(ShaderInput(uniform.second->GetName(),
                                             uniform.second->GetShaderDataType(),
                                             uniform.second->GetIndex(),
                                             uniform.second->GetTextureSize(),
                                             uniform.second->GetOffset()));
            }
        }

        for (auto uniform: m_program->GetDefaultUniforms()) {
            if (!uniform.second->IsTexture()) continue;
            inputs.push_back(ShaderInput(uniform.second->GetName(),
                                         uniform.second->GetShaderDataType(),
                                         uniform.second->GetIndex(),
                                         uniform.second->GetTextureSize(),
                                         uniform.second->GetOffset()));
        }
        
        return inputs;
    }
}
