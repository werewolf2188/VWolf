//
//  OpenGLProgram.h
//  VWolf
//
//  Created by Enrique Moises on 10/11/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Core/Bindable.h"

#include "VWolf/Core/Render/RHI/Program.h"
#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Core/Render/Mesh.h"

#include "OpenGLBuffer.h"
#include "OpenGLBufferSet.h"

#include "VWolf/Platform/DXILHelpers.h"

namespace VWolf::RHI {

    class OpenGLAttribute {
    public:
        OpenGLAttribute(GLuint programId, GLint index);
        ~OpenGLAttribute() = default;
        
        std::string GetName() { return name; }
        GLenum GetType() { return type; }
        GLint GetIndex() { return index; }
        GLint GetSize() { return size; }
        
        ShaderDataType GetShaderDataType();        
        AttributeFormat GetAttributeFormat();
        uint32_t GetDimension();
    private:
        GLint index;
        std::string name;
        GLenum type;
        GLint size;
    };

    class OpenGLUniform {
    public:
        OpenGLUniform(GLuint programId, GLint index);
        ~OpenGLUniform() = default;

        std::string GetName() { return name; }

        void SetIndex(GLint index) { this->index = index; }
        void SetType(GLint type) { this->type = type; }
        void SetOffset(GLint offset) { this->offset = offset; }

        GLint GetIndex() { return index; }
        GLuint GetType() { return type; }
        GLuint GetOffset() { return offset; }

        ShaderDataType GetShaderDataType();
        bool IsTexture();
        int GetTextureSize();
    private:
        GLint index;
        std::string name;
        GLint type;
        GLint offset;
    };

    class OpenGLUniformBuffer {
    public:

        OpenGLUniformBuffer(GLuint programId, GLint index);
        ~OpenGLUniformBuffer();

        std::string GetName() { return name; }
        GLint GetIndex() { return index; }
        GLint GetBinding() { return binding; }
        GLint GetSize() { return size; }
        std::map<std::string, Ref<OpenGLUniform>> GetUniforms() { return uniforms; }

        // TODO: Remove later
        void ReplaceProgramBinding(const Ref<BindableProgramArgument> argument, const ProgramArgumentBindingValue& value);
        
        static void AddTotalBindings(int bindings);
    private:
        void Build(GLuint programId);
        void RetrieveUniforms(GLuint programId);
    private:
        GLint binding;
        GLint size;
        GLint index;
        std::string name;
        std::map<std::string, Ref<OpenGLUniform>> uniforms;
    private:
        static int totalBindings;
    };

    class OpenGLVertexArray: public Object, public Bindable {
    public:
        OpenGLVertexArray(std::map<std::string, DXIL::Attribute> attributes, Ref<OpenGLBufferSet> vertexSet, Ref<OpenGLBuffer> indexBuffer);
        ~OpenGLVertexArray();
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        Ref<OpenGLBufferSet> GetVertexSet() { return vertexSet; }
        Ref<OpenGLBuffer> GetIndexBuffer() { return indexBuffer; }
    private:
        void Build();
        
        unsigned int vertexArrayId = 0;

        std::map<std::string, DXIL::Attribute> attributes;
        Ref<OpenGLBufferSet> vertexSet;
        Ref<OpenGLBuffer> indexBuffer;
    };

    class OpenGLProgram: public Program, public Bindable {
    public:
        OpenGLProgram(Weak<Context> context, const ProgramSettings& settings);
        ~OpenGLProgram();
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        // TODO: This will change
//        void SetData(const void* data, const char* name, uint32_t size, uint32_t offset);
        void ReplaceProgramBinding(const Ref<BindableProgramArgument> argument, const ProgramArgumentBindingValue& value);
        // TODO: This will be better
//        void BindBuffer(Ref<BindableProgramArgument> argument, ProgramArgumentBindingValue& value);
        
        std::map<std::string, Ref<OpenGLUniformBuffer>> GetUniformBuffers() { return uniformBuffers; }
        std::map<std::string, Ref<OpenGLUniform>> GetDefaultUniforms() { return defaultUniforms; }
        std::map<std::string, DXIL::Attribute> GetAttributes() { return dxilAttributes; }
        
        operator GLuint() const { return programId; }
        
        Ref<OpenGLVertexArray> CreateVertexArray(Ref<OpenGLBufferSet> vertexSet, Ref<OpenGLBuffer> indexBuffer);
    private:
        bool IsLinked() { return GetStatus(GL_LINK_STATUS) == GL_TRUE; }
        
        void RetrieveUniformsBlocks();
        void RetrieveUniforms();
        void RetrieveAttributes();
        
        void Initialize();
        GLint GetStatus(GLenum enumValue);
        void PrintErrorAndAssert();
        
        GLuint programId;
        std::map<std::string, Ref<OpenGLUniformBuffer>> uniformBuffers;
        std::map<std::string, Ref<OpenGLUniform>> defaultUniforms;
        std::map<std::string, Ref<OpenGLAttribute>> attributes;
        std::map<std::string, DXIL::Attribute> dxilAttributes;
    };
}
