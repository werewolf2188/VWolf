//
//  MetalShader.cpp
//  VWolf
//
//  Created by Enrique Moises on 9/1/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "VWolf/Platform/DXILHelpers.h"
#include "MetalShader.h"

namespace VWolf::RHI {
    MetalShader::MetalShader(Weak<Context> context, ShaderType type, const ShaderSettings& settings):
    Shader(context, type, settings) {
        dxil = CreateRef<VWolf::DXIL::Shader>(VWolf::DXIL::Shader(settings.GetShaderName(),
                                                                  settings.GetEntryFunction().GetFuncionName(),
                                                                  (VWolf::ShaderType)(uint32_t)type,
                                                                  settings.GetCode(),
                                                                  DXIL::Shader::ArgumentType::Metal));
    }

    MetalShader::~MetalShader() {}
}
#endif
