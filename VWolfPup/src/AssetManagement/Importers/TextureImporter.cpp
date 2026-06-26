//
//  TextureImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/26/26.
//

#include "TextureImporter.h"

namespace VWolfPup {
    uint32_t TextureImporter::LoadPriority(std::filesystem::path path) const {
        return 2;
    }

    bool TextureImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        if (path == Extension("HDR Files", ".hdr")) {
            
        } else {
            
        }
        return true;
    }

    const std::vector<Extension> TextureImporter::Extensions() const {
        return {
            Extension("PNG Files", ".png"),
            Extension("GIF Files", ".gif"),
            Extension("JPEG Files", ".jpg"),
            Extension("JPEG Files", ".jpeg"),
            Extension("HDR Files", ".hdr")
        };
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(TextureImporter);
}
