//
//  TextureImporter.h
//  VWolfPup
//
//  Created by Enrique Moises on 6/26/26.
//

#pragma once

#include "AssetImporter.h"

namespace VWolfPup {
    class TextureImporter: public AssetImporter {
    public:
        uint32_t LoadPriority(std::filesystem::path path) const override;
        bool Import(std::filesystem::path path, VWolf::UUID _id) override;
    protected:
        const std::vector<Extension> Extensions() const override;
    private:
        VWolf::TextureOptions options;
        
        BOOST_DESCRIBE_CLASS(TextureImporter, (AssetImporter), (), (), (options))
        VWOLF_SERIALIZATION_FRIENDS(TextureImporter)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolfPup::TextureImporter);
}
