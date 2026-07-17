//
//  FileTemplateImporter.h
//  VWolfPup
//
//  Created by Enrique Moises on 7/14/26.
//

#pragma once

#include "AssetImporter.h"

namespace VWolfPup {
    class FileTemplateImporter: public AssetImporter {
    public:
        uint32_t LoadPriority(std::filesystem::path path) const override;
        bool Import(std::filesystem::path path, VWolf::UUID _id) override;
    protected:
        const std::vector<Extension> Extensions() const override;
    private:
        BOOST_DESCRIBE_CLASS(FileTemplateImporter, (AssetImporter), (), (), ())
        VWOLF_SERIALIZATION_FRIENDS(FileTemplateImporter)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolfPup::FileTemplateImporter);
}
