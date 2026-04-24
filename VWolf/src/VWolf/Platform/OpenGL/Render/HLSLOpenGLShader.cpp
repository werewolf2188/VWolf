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

// https://github.com/werewolf2188/SPIRV-Cross
#include <spirv_glsl.hpp>

namespace VWolf {
    class HLOGLShaderSource {
    public:
        HLOGLShaderSource(ShaderSource source) {
            shader = DXIL::Shader(source, DXIL::Shader::ArgumentType::OpenGL);
            TranslateFromDXILToGLSL();
            type = ShaderTypeEquivalent(shader.GetType());
            shaderId = glCreateShader(type);
            GLThrowIfFailedNoAction("glCreateShader");
            Compile();
        }
        
        HLOGLShaderSource(Stage source, std::string code) {
            shader = DXIL::Shader(source, code, DXIL::Shader::ArgumentType::OpenGL);
            TranslateFromDXILToGLSL();
            type = ShaderTypeEquivalent(shader.GetType());
            shaderId = glCreateShader(type);
            GLThrowIfFailedNoAction("glCreateShader");
            Compile();
        }
        
        ~HLOGLShaderSource() {
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

        friend class HLOGLProgram;
    private:
        void TranslateFromDXILToGLSL() {
            SmartPoint<IDxcBlob>& blob = shader.GetShader();
            uint32_t* pointer = reinterpret_cast<uint32_t*>(blob->GetBufferPointer());
            size_t di = blob->GetBufferSize() / sizeof(uint32_t);
            std::vector<uint32_t> spirv(pointer, pointer + di);
            spirv_cross::CompilerGLSL glsl(std::move(spirv));
            glsl.build_combined_image_samplers();
            // The SPIR-V is now parsed, and we can perform reflection on it.
            spirv_cross::ShaderResources resources = glsl.get_shader_resources();

            // Get all sampled images in the shader.
            for (auto &resource : resources.stage_inputs)
            {
                if (shader.GetType() == ShaderType::Vertex) {
                    std::string new_name = SanitizeNameForVertexForInput(resource.name);
                    glsl.set_name(resource.id, new_name);
                } else if (shader.GetType() == ShaderType::Fragment) {
                    std::string new_name = SanitizeNameForVertexForOutputOrFragmentForInput(resource.name);
                    glsl.set_name(resource.id, new_name);
                }

            }
            
            for (auto &resource : resources.stage_outputs)
            {
                if (shader.GetType() == ShaderType::Vertex) {
                    std::string new_name = SanitizeNameForVertexForOutputOrFragmentForInput(resource.name);
                    glsl.set_name(resource.id, new_name);
                }
            }

            uint32_t index = 0;
            for (auto &resource : resources.sampled_images)
            {
                auto& texture = resources.separate_images[index];
                std::string name = texture.name;
                std::cout << name << std::endl;
                glsl.set_name(resource.id, name);
                index++;
            }

            // Set some options.
            spirv_cross::CompilerGLSL::Options options;
            options.version = OPENGL_VERSION;
            options.enable_420pack_extension = false;
            glsl.set_common_options(options);
            

            // Compile to GLSL, ready to give to GL driver.
            sourceText = glsl.compile();
        }
        
        std::string SanitizeNameForVertexForInput(std::string name) {
            if (name.rfind("in.var.", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(7); // Remove "in"
                
                // 3. Rename the variable
                return new_name;
            }
            return name;
        }
        
        std::string SanitizeNameForVertexForOutputOrFragmentForInput(std::string name) {
            if (name.rfind("in.var.a", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(8); // Remove "in"
                
                // 3. Rename the variable
                return "v" + new_name;
            } if (name.rfind("in.var.", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(7); // Remove "in"
                
                // 3. Rename the variable
                return "v_" + new_name;
            } else if (name.rfind("out.var.a", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(9); // Remove "in"
                
                // 3. Rename the variable
                return "v" + new_name;
            } else if (name.rfind("out.var.", 0) == 0) { // C++17 starts_with
                std::string new_name = name.substr(8); // Remove "in"
                
                // 3. Rename the variable
                return "v_" + new_name;
            }
            return name;
        }
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
                default:
                    VWOLF_CORE_ASSERT("Incorrect Shader Type");
            }
            return -1;
        }
    private:
        DXIL::Shader shader;
        std::string sourceText;
        
        GLuint shaderId;
        GLenum type;
        bool isAttached;
    };

    class HLOGLAttribute {
    public:
        HLOGLAttribute(GLuint programId, GLint index) {
            GLint length = 255;
            GLchar* nameHolder = new GLchar[length];
            GLThrowIfFailed(glGetActiveAttrib(programId, index, length, 0, &size, &type, nameHolder));
            name = std::string(nameHolder);
            delete[] nameHolder;
        }

        ~HLOGLAttribute() = default;
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

    class HLOGLUniform {
    public:
        HLOGLUniform(GLuint programId, GLint index): index(index) {
            GLint length;
            GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length));
            char* nameHolder = new char[length];
            GLThrowIfFailed(glGetActiveUniformName(programId, index, length, 0, nameHolder));
            name = std::string(nameHolder);
            delete[] nameHolder;
        }

        ~HLOGLUniform() = default;

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

    class HLOGLUniformBuffer {
    public:

        HLOGLUniformBuffer(GLuint programId, GLint index): index(index) {
            GLThrowIfFailed(glGenBuffers(1, &id));
            Build(programId);
        }

        ~HLOGLUniformBuffer() {
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
        
        std::map<std::string, Ref<HLOGLUniform>> GetUniforms() {
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
                Ref<HLOGLUniform> uniform = CreateRef<HLOGLUniform>(programId, ubActiveUniformsIndices[uIndex]);
                uniform->SetType(ubActiveUniformsTypes[uIndex]);
                uniform->SetOffset(ubActiveUniformsOffsets[uIndex]);
                uniforms.insert(std::pair<std::string, Ref<HLOGLUniform>>(uniform->GetName(), uniform));
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
        std::map<std::string, Ref<HLOGLUniform>> uniforms;
    private:
        static int totalBindings;
    };

    int HLOGLUniformBuffer::totalBindings = 0;

    class HLOGLProgram {
    public:
        HLOGLProgram(): programId(glCreateProgram()) {
            GLThrowIfFailedNoAction("glCreateProgram");
        }
        ~HLOGLProgram() {
            Clear();
            GLThrowIfFailed(glDeleteProgram(programId));
        }

        void AddShader(Ref<HLOGLShaderSource> source) {
            sources.push_back(source);
        }

        void RemoveShader(Ref<HLOGLShaderSource> source) {
            auto lambda = [source](Ref<HLOGLShaderSource> it) {
                return it->shaderId == source->shaderId;
            };
            Ref<HLOGLShaderSource> found = *std::remove_if(sources.begin(), sources.end(), lambda);
            DettachShader(found);
        }

        void Clear() {
            DettachShaders();
            sources.clear();
        }

        void AttachShader(Ref<HLOGLShaderSource> source) {
            AddShader(source);
            AttachShaderWithoutAdding(source);
        }

        void AttachShaders() {
            for (Ref<HLOGLShaderSource> source: sources) {
                if (!source->isAttached)
                    AttachShaderWithoutAdding(source);
            }
        }

        void DettachShader(Ref<HLOGLShaderSource> source) {
            GLThrowIfFailed(glDetachShader(programId, source->shaderId));
            source->isAttached = false;
        }

        void DettachShaders() {
            for (Ref<HLOGLShaderSource> source: sources) {
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
                Ref<HLOGLUniformBuffer> ubBlock = CreateRef<HLOGLUniformBuffer>(*this, index);
                uniformBuffers.insert(std::pair<std::string, Ref<HLOGLUniformBuffer>>(ubBlock->GetName(), ubBlock));
            }
            HLOGLUniformBuffer::AddTotalBindings(ubCount);
            RetrieveDefaultUniforms();
            RetrieveAttributes();
        }

        void SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
            Ref<HLOGLUniformBuffer> ubBlock = 0;
            auto lambda = [name](std::pair<std::string, Ref<HLOGLUniformBuffer>> it) {
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

        std::vector<Ref<HLOGLShaderSource>> GetSources() {
            return sources;
        }

        std::map<std::string, Ref<HLOGLUniformBuffer>> GetUniformBuffers() {
            return uniformBuffers;
        }

        std::map<std::string, Ref<HLOGLUniform>> GetDefaultUniforms() {
            return defaultUniforms;
        }

        std::map<std::string, Ref<HLOGLAttribute>> GetAttributes() {
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
    
        void AttachShaderWithoutAdding(Ref<HLOGLShaderSource> source) {
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
                Ref<HLOGLUniform> uniform = CreateRef<HLOGLUniform>(programId, uIndex);
                uniform->SetType(uActiveUniformsTypes[uIndex]);
                uniform->SetOffset(uActiveUniformsOffsets[uIndex]);
#ifdef VWOLF_PLATFORM_WINDOWS
                GLuint _index = glGetUniformLocation(programId, uniform->GetName().c_str());
                if (uIndex != _index)
                    uniform->SetIndex(_index);
#endif
                defaultUniforms.insert(std::pair<std::string, Ref<HLOGLUniform>>(uniform->GetName(), uniform));
            }
            delete[] uActiveUniformsOffsets;
            delete[] uActiveUniformsTypes;
        }

        void RetrieveAttributes() {
            GLint attributesCount;
            GLThrowIfFailed(glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &attributesCount));
            for(int aIndex = 0; aIndex < attributesCount; aIndex++) {
                Ref<HLOGLAttribute> attribute = CreateRef<HLOGLAttribute>(programId, aIndex);
                attributes.insert(std::pair<std::string, Ref<HLOGLAttribute>>(attribute->GetName(), attribute));
            }
        }
    private:
        GLuint programId;
        std::vector<Ref<HLOGLShaderSource>> sources;
        std::map<std::string, Ref<HLOGLUniformBuffer>> uniformBuffers;
        std::map<std::string, Ref<HLOGLUniform>> defaultUniforms;
        std::map<std::string, Ref<HLOGLAttribute>> attributes;
    };

    HLSLOpenGLShader::HLSLOpenGLShader(std::string name,
                                       std::initializer_list<ShaderSource> otherShaders,
                                       ShaderConfiguration configuration): PShader(name,
                                                                                   otherShaders,
                                                                                   configuration)
    {
        m_program = CreateRef<HLOGLProgram>();
        for(ShaderSource source: m_otherShaders)
           m_program->AttachShader(CreateRef<HLOGLShaderSource>(source));
        m_program->Link();
        //        m_program->Validate();
        m_program->DettachShaders();
        m_program->RetrieveUniforms();
    }

    HLSLOpenGLShader::HLSLOpenGLShader(Shader& coreShader): PShader(coreShader) {
        m_program = CreateRef<HLOGLProgram>();
        for(Stage source: coreShader.GetSubShader().GetStages())
           m_program->AttachShader(CreateRef<HLOGLShaderSource>(source, coreShader.GetSubShader().GetCode()));
        m_program->Link();
        //        m_program->Validate();
        m_program->DettachShaders();
        m_program->RetrieveUniforms();
        loadFromNewShader = true;
        settings = coreShader.GetSettings();
    }

    HLSLOpenGLShader::~HLSLOpenGLShader() {
        
    }

    void HLSLOpenGLShader::Bind() const {
        m_program->Bind();
        // Configuration
        SetConfiguration();
    }

    void HLSLOpenGLShader::Unbind() const {
        m_program->Unbind();
    }

    std::vector<Ref<ShaderInput>> HLSLOpenGLShader::GetMaterialInputs() const {
        std::vector<Ref<ShaderInput>> inputs;
        std::string cbMaterialName = std::string("type_cbPer") + std::string(materialName);
        const char * _materialName = cbMaterialName.c_str();
        auto lambda = [_materialName](std::pair<std::string, Ref<HLOGLUniformBuffer>> it) {
            return strcmp(it.first.c_str(), _materialName) == 0;
        };
        auto container = m_program->GetUniformBuffers();
        auto mat = std::find_if(container.begin(),
                                container.end(),
                                lambda);
        if (mat == container.end()) return inputs;

        Ref<HLOGLUniformBuffer> material = mat->second;
        
        if (material == nullptr) {
            return inputs;
        }
        for (std::pair<std::string, Ref<HLOGLUniform>> uniform: material->GetUniforms()) {
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
        auto lambda = [_materialName](std::pair<std::string, Ref<HLOGLUniformBuffer>> it) {
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

    std::vector<ShaderInput> HLSLOpenGLShader::GetTextureInputs() const {
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

    std::string HLSLOpenGLShader::GetName() const {
        return this->m_name;
    }

    void HLSLOpenGLShader::SetData(const void* data, const char* name, uint32_t size, uint32_t offset) {
        std::string cbMaterialName = std::string("type_cbPer") + std::string(name);
        m_program->SetData(data, cbMaterialName.c_str(), size, offset);
    }

    void HLSLOpenGLShader::SetConfiguration() const {
        SetRasterization();
        SetBlend();
        SetDepthStencil();
    }

    void HLSLOpenGLShader::SetRasterization() const {
        // Rasterization
        bool cullEnabled = loadFromNewShader ? settings.GetRasterization().GetCullEnabled() : m_configuration.rasterization.cullEnabled;
        CullMode cullMode = loadFromNewShader ? settings.GetRasterization().GetCullMode() : (CullMode)m_configuration.rasterization.cullMode;
        FillMode fillMode = loadFromNewShader ? settings.GetRasterization().GetFillMode() : (FillMode)m_configuration.rasterization.fillMode;
        bool counterClockwise = loadFromNewShader ? settings.GetRasterization().GetCounterClockwise() : m_configuration.rasterization.counterClockwise;
        
        if (cullEnabled) {
            GLThrowIfFailed(glEnable(GL_CULL_FACE));
        }
        else {
            GLThrowIfFailed(glDisable(GL_CULL_FACE));
        }

        switch (cullMode) {
            case CullMode::Front:
                GLThrowIfFailed(glCullFace(GL_FRONT));
                break;
            case CullMode::Back:
                GLThrowIfFailed(glCullFace(GL_BACK));
                break;
            case CullMode::FrontAndBack:
                GLThrowIfFailed(glCullFace(GL_FRONT_AND_BACK));
                break;
        }

        switch (fillMode) {
            case FillMode::Wireframe:
                GLThrowIfFailed(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                break;
            case FillMode::Solid:
                GLThrowIfFailed(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                break;
        }

        GLThrowIfFailed(glFrontFace(counterClockwise ? GL_CCW: GL_CW));
    }

    GLuint HLSLOpenGLShader::GetBlendFunction(BlendFunction function) const {
        switch (function) {
            case BlendFunction::Zero: return GL_ZERO;
            case BlendFunction::One: return GL_ONE;
            case BlendFunction::SrcColor: return GL_SRC_COLOR;
            case BlendFunction::InvSrcColor: return GL_ONE_MINUS_SRC_COLOR;
            case BlendFunction::DstColor: return GL_DST_COLOR;
            case BlendFunction::InvDstColor: return GL_ONE_MINUS_DST_COLOR;
            case BlendFunction::SrcAlpha: return GL_SRC_ALPHA;
            case BlendFunction::InvSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case BlendFunction::DstAlpha: return GL_DST_ALPHA;
            case BlendFunction::InvDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
            case BlendFunction::Src1Color: return GL_SRC1_COLOR;
            case BlendFunction::InvSrc1Color: return GL_ONE_MINUS_SRC1_COLOR;
            case BlendFunction::Src1Alpha: return GL_SRC1_ALPHA;
            case BlendFunction::InvSrc1Alpha: return GL_ONE_MINUS_SRC1_ALPHA;
            case BlendFunction::SrcAlphaSat: return GL_SRC_ALPHA_SATURATE;
            case BlendFunction::CnstColor: return GL_CONSTANT_COLOR;
            case BlendFunction::InvCnstColor: return GL_ONE_MINUS_CONSTANT_COLOR;
            case BlendFunction::CnstAlpha: return GL_CONSTANT_ALPHA;
            case BlendFunction::InvCnstAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
        }
        return GL_ZERO;
    }

    void HLSLOpenGLShader::SetBlend() const {
        // Blend
        bool enabled = loadFromNewShader ? settings.GetBlend().GetEnabled() : m_configuration.blend.enabled;
        BlendEquation equation = loadFromNewShader ? settings.GetBlend().GetEquation() : (BlendEquation)m_configuration.blend.equation;
        BlendFunction sourceFunc = loadFromNewShader ? settings.GetBlend().GetSourceFunction() : (BlendFunction)m_configuration.blend.sourceFunction;
        BlendFunction destinationFunc = loadFromNewShader ? settings.GetBlend().GetDestinationFunction() : (BlendFunction)m_configuration.blend.destinationFunction;
        
        if (enabled) {
            GLThrowIfFailed(glEnable(GL_BLEND));
        }
        else {
            GLThrowIfFailed(glDisable(GL_BLEND));
        }

        GLuint sourceFunction, destinationFunction;
        sourceFunction = GetBlendFunction(sourceFunc);
        destinationFunction = GetBlendFunction(destinationFunc);
        GLThrowIfFailed(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, sourceFunction, destinationFunction));
        
        switch (equation) {
            case BlendEquation::Add:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_ADD));
                break;
            case BlendEquation::Substract:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_SUBTRACT));
                break;
            case BlendEquation::ReverseSubstract:
                GLThrowIfFailed(glBlendEquation(GL_FUNC_REVERSE_SUBTRACT));
                break;
            case BlendEquation::Min:
                GLThrowIfFailed(glBlendEquation(GL_MIN));
                break;
            case BlendEquation::Max:
                GLThrowIfFailed(glBlendEquation(GL_MAX));
                break;
        }
    }

    void HLSLOpenGLShader::SetDepthStencil() const {
        // Depth/Stencil
        bool depthTest = loadFromNewShader ? settings.GetDepthStencil().GetDepthTest() : m_configuration.depthStencil.depthTest;
        DepthFunction depthFunction = loadFromNewShader ? settings.GetDepthStencil().GetDepthFunction() : (DepthFunction)m_configuration.depthStencil.depthFunction;
        
        if (depthTest) {
            GLThrowIfFailed(glEnable(GL_DEPTH_TEST));
        }
        else {
            GLThrowIfFailed(glDisable(GL_DEPTH_TEST));
        }

        switch(depthFunction) {
            case DepthFunction::Never:
                GLThrowIfFailed(glDepthFunc(GL_NEVER));
                break;
            case DepthFunction::Less:
                GLThrowIfFailed(glDepthFunc(GL_LESS));
                break;
            case DepthFunction::LEqual:
                GLThrowIfFailed(glDepthFunc(GL_LEQUAL));
                break;
            case DepthFunction::Equal:
                GLThrowIfFailed(glDepthFunc(GL_EQUAL));
                break;
            case DepthFunction::NotEqual:
                GLThrowIfFailed(glDepthFunc(GL_NOTEQUAL));
                break;
            case DepthFunction::GEqual:
                GLThrowIfFailed(glDepthFunc(GL_GEQUAL));
                break;
            case DepthFunction::Greater:
                GLThrowIfFailed(glDepthFunc(GL_GREATER));
                break;
            case DepthFunction::Always:
                GLThrowIfFailed(glDepthFunc(GL_ALWAYS));
                break;
        }
    }
}
