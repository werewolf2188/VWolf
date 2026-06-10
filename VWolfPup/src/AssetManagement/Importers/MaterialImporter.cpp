//
//  MaterialImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/6/26.
//

#include "MaterialImporter.h"

namespace VWolfPup {
    uint32_t MaterialImporter::LoadPriority(std::filesystem::path path) const {
        return 2;
    }

    bool MaterialImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        return true;
    }


    const std::vector<Extension> MaterialImporter::Extensions() const {
        return {
            Extension("Material", ".vwolfmat")
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(MaterialImporter);
}
