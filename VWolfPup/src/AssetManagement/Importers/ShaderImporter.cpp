//
//  ShaderImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/6/26.
//

#include "ShaderImporter.h"

namespace VWolfPup {
    uint32_t ShaderImporter::LoadPriority() {
        return 0;
    }

    const std::vector<Extension> ShaderImporter::Extensions() const {
        return {
            Extension("Shader", ".vwolfshader"),
            Extension("Shader Dependency", ".hlsli")
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(ShaderImporter);
}
