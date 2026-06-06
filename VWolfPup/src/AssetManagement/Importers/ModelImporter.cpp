//
//  ModelImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/8/26.
//

#include "ModelImporter.h"

namespace VWolfPup {
    uint32_t ModelImporter::LoadPriority() {
        return 1;
    }

    const std::vector<Extension> ModelImporter::Extensions() const {
        return {
            Extension("Object Model", ".obj")
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(ModelImporter);
}
