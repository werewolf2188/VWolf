//
//  MetalShader.h
//  VWolf
//
//  Created by Enrique Moises on 9/1/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/Shader.h"

namespace VWolf::DXIL {
    class Shader;
}

namespace VWolf::RHI {
    class MetalShader: public Shader {
    public:
        MetalShader(Weak<Context> context, ShaderType type, const ShaderSettings& settings);
        ~MetalShader();
        
        const Ref<VWolf::DXIL::Shader> GetDXILCompiledShader() const { return dxil; }
    private:
        Ref<VWolf::DXIL::Shader> dxil;
    };
}
