//
//  Shader.cpp
//  VWolf
//
//  Created by Enrique Moises on 4/23/26.
//

#include "vwpch.h"
#include "Shader.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::ShaderType, Vertex, Pre_Tesselator, Post_Tesselator, Geometry, Fragment)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::FillMode, Wireframe, Solid);
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::CullMode, Back, Front, FrontAndBack)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::DepthFunction, Never, Less, Equal, LEqual, Greater, NotEqual, GEqual, Always)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::BlendEquation, Add, Substract, ReverseSubstract, Min, Max)
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::BlendFunction, Zero, One, SrcColor, InvSrcColor, DstColor, InvDstColor, SrcAlpha, InvSrcAlpha, DstAlpha, InvDstAlpha, Src1Color, InvSrc1Color, Src1Alpha, InvSrc1Alpha, SrcAlphaSat, CnstColor, InvCnstColor, CnstAlpha, InvCnstAlpha)

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Rasterization)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::DepthStencil)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Blend)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Stage)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::SubShader)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Settings)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Shader)
}

namespace VWolf {
    Shader::Shader(std::filesystem::path path) {
        Deserialize(path);
    }

    void Shader::Deserialize(std::filesystem::path path) {
        constexpr const char * key = "shader";
        
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CORE_ERROR("Failed to load .material file '%s'\n     %s", path.string().c_str(), e.what());
        }
        (*this) = data[key].as<Shader>();
        std::cout << this->name << std::endl;
//        internalShader = LoadPlatformShader(*this);
    }
}
