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
}
