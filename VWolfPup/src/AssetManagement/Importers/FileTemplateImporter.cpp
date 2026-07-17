//
//  FileTemplateImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 7/14/26.
//

#include "FileTemplateImporter.h"
#include "../../ProjectManagement/FileTemplate.h"

namespace VWolfPup {
    uint32_t FileTemplateImporter::LoadPriority(std::filesystem::path path) const {
        return 3;
    }

    bool FileTemplateImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        FileTemplate::Load(path, _id);
        return true;
    }

    const std::vector<Extension> FileTemplateImporter::Extensions() const {
        return {
            Extension("Template File", ".vwolftemplate")
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(FileTemplateImporter);
}
