//
//  ShaderImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/6/26.
//

#include "ShaderImporter.h"

namespace VWolfPup {

    Extension ShaderImporter::shaderExtension("Shader", ".vwolfshader");
    Extension ShaderImporter::shaderLibraryExtension("Shader Dependency", ".hlsli");

    uint32_t ShaderImporter::LoadPriority() {
        return 0;
    }

    bool ShaderImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        if (shaderExtension == path.extension().string()) {
            VWolf::Shader::LoadShader(path, _id);
        }
        return true;
    }

    const std::vector<Extension> ShaderImporter::Extensions() const {
        return {
            shaderExtension,
            shaderLibraryExtension
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(ShaderImporter);
}
