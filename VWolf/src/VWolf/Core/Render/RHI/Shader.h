//
//  Shader.h
//  VWolf
//
//  Created by Enrique Moises on 8/30/26.
//

#pragma once

namespace VWolf::RHI {
    enum class ShaderType {
        Vertex = 1,
        PreTesselator = 2,
        PostTesselator = 4,
        Geometry = 8,
        Fragment = 16,
        Compute = 32
    };

    class ShaderEntryFunction {
    public:
        ShaderEntryFunction() = default;
        ShaderEntryFunction(std::string funcionName):
        funcionName(funcionName) {}
        
        const std::string& GetFuncionName() const { return funcionName; }
    private:
        std::string funcionName;
    };

    class ShaderSettings {
    public:
        ShaderSettings() = default;
        ShaderSettings(ShaderEntryFunction entryFunction,
                       const std::string& shaderName,
                       const std::string& code):
        entryFunction(entryFunction), shaderName(shaderName), code(code) {}
        
        const ShaderEntryFunction& GetEntryFunction() const { return entryFunction; }
        const std::string& GetShaderName() const { return shaderName; }
        const std::string& GetCode() const { return code; }
    private:
        ShaderEntryFunction entryFunction;
        std::string shaderName;
        std::string code;
    };

    class Context;

    class Shader {
    public:
        Shader(Weak<Context> context, ShaderType type, const ShaderSettings& settings): context(context), type(type), settings(settings) {}
        virtual ~Shader() = default;
        
        const Weak<Context> GetContext() const { return context; }
        const ShaderSettings& GetSettings() const { return settings; }
        const ShaderType& GetType() const { return type; }
    private:
        Weak<Context> context;
        ShaderType type;
        ShaderSettings settings;
    };
}
