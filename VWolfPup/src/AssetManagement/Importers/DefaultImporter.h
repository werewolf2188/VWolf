//
//  DefaultImporter.hpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/7/26.
//

#pragma once

#include "AssetImporter.h"

namespace VWolfPup {
    class DefaultImporter: public AssetImporter {
    public:
        uint32_t LoadPriority() override;
    protected:
        const std::vector<Extension> Extensions() const override;
    public:
        const bool CanImport(std::filesystem::path path) const override;
    private:
        BOOST_DESCRIBE_CLASS(DefaultImporter, (AssetImporter), (), (), ())
        VWOLF_SERIALIZATION_FRIENDS(DefaultImporter)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolfPup::DefaultImporter);
}
