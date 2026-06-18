#include "vwpch.h"
#include "VWolf/Core/Application.h"

#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Platform/PShader.h"

#include "VWolf/Platform/OpenGL/Render/HLSLOpenGLShader.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/DirectX12/Render/HLSLShader.h"
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "VWolf/Platform/Metal/Render/HLSLMetalShader.h"
#endif

#include <boost/algorithm/string.hpp>

#ifdef ATTRIBUTE_LOOKUP
#undef ATTRIBUTE_LOOKUP
#endif
#define ATTRIBUTE_LOOKUP(name, search) \
(name.find(#search) != std::string::npos)

namespace VWolf {
    Ref<PShader> LoadPlatformShader(Shader& coreShader) {
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                return CreateRef<HLSLOpenGLShader>(coreShader);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                return CreateRef<HLSLShader>(coreShader);
                break;
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                return CreateRef<HLSLMetalShader>(coreShader);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Shader: Not yet implemented");
        }

        return nullptr;
    }

    std::vector<Ref<ShaderInput>> GetMaterialInputs(Ref<PShader> pshader) {
        return pshader->GetMaterialInputs();
    }

    size_t GetMaterialSize(Ref<PShader> pshader) {
        return pshader->GetMaterialSize();
    }

    std::vector<ShaderInput> GetTextureInputs(Ref<PShader> pshader) {
        return pshader->GetTextureInputs();
    }

    Attribute PShader::GetAttributeFromName(std::string name) const {
        /*
         Position,
         Normal,
         Tangent,
         Color,
         TextCoord0,
         TextCoord1,
         TextCoord2,
         TextCoord3,
         TextCoord4,
         TextCoord5,
         TextCoord6,
         TextCoord7,
         BlendWeight,
         BlendIndices
         
         float3 PosL  : a_Position;
         float4 Color : a_Color;
         float3 Normal  : a_Normal;
         float3 Tangent  : a_Tangent;
         float2 TexCoord  : a_TexCoord;
         */
        
        // TODO: Rename semantics in the shaders
        if (ATTRIBUTE_LOOKUP(boost::to_upper_copy(name), POSITION)) return Attribute::Position;
        else if (ATTRIBUTE_LOOKUP(boost::to_upper_copy(name), COLOR)) return Attribute::Color;
        else if (ATTRIBUTE_LOOKUP(boost::to_upper_copy(name), NORMAL)) return Attribute::Normal;
        else if (ATTRIBUTE_LOOKUP(boost::to_upper_copy(name), TANGENT)) return Attribute::Tangent;
        else if (ATTRIBUTE_LOOKUP(boost::to_upper_copy(name), TEXCOORD)) return Attribute::TextCoord0;
        return Attribute::Unknown;
    }
}
