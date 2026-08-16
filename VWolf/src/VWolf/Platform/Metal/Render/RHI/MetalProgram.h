//
//  MetalProgram.h
//  VWolf
//
//  Created by Enrique Moises on 9/2/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/Program.h"

namespace VWolf {
    enum class AttributeFormat;
}

namespace VWolf::RHI {

    struct StageInAttribute {
    public:
        StageInAttribute(boost::json::value value);

        int64_t GetIndex();
        MTL::VertexFormat GetVertexFormat();
        AttributeFormat GetAttributeFormat();
        uint32_t GetDimension();
        std::string GetName();
        uint64_t GetSize();
    private:
        boost::json::value value;
    };

    class Argument {
    public:
        Argument();
        Argument(boost::json::value value, ShaderType shaderType);
        Argument(const Argument& argument);
        Argument(Argument&& argument);
    public:
        Argument& operator=(const Argument& argument);
    public:
        uint64_t GetIndex();
        uint64_t GetSpace();
        std::string GetName();
        uint64_t GetSize();
        int64_t GetOffset();
        IRResourceType GetType();
        ShaderType GetShaderType();
    private:
        boost::json::value value;
        ShaderType shaderType;
    };

    class MetalProgram: public Program {
    public:
        MetalProgram(Weak<Context> context, const ProgramSettings& settings);
        ~MetalProgram();
        
        virtual void SetName(const std::string& name) override;
        
        const std::vector<Ref<StageInAttribute>>& GetStageInAttributes() const { return stageInAttributes; }
        const std::map<ShaderType, std::map<std::string, Ref<Argument>>>& GetTopLevelArguments() const { return topLevelArguments; }
        const std::map<std::string, Ref<MTL::Function>>& GetFunctions() const { return functions; }
    private:
        std::map<std::string, Ref<MTL::Library>> CompileDXILUsingIR();
        void ExtractFunctions(std::map<std::string, Ref<MTL::Library>> libraries);
        
        std::map<std::string, Ref<MTL::Function>> functions;
        std::vector<Ref<StageInAttribute>> stageInAttributes;
        std::map<ShaderType, std::map<std::string, Ref<Argument>>> topLevelArguments;
        
        // TODO: Refactor
        std::map<ShaderType, std::vector<MTL::Buffer*>> pDescriptorTables;
        //
        
        friend class MetalRenderCommandList;
    };
}
