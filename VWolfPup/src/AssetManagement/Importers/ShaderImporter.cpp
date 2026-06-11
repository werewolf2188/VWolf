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

    uint32_t ShaderImporter::LoadPriority(std::filesystem::path path) const {
        if (shaderExtension == path) {
            return 1;
        } else return 0;
    }

    bool ShaderImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        if (shaderExtension == path) {
            VWolf::Shader::LoadShader(path, _id);
        } else if (shaderLibraryExtension == path) {
            VWolf::Shader::LoadShaderLibrary(path, _id);
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
