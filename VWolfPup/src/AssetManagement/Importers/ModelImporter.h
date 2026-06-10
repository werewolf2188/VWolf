//
//  ModelImporter.hpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/8/26.
//

#pragma once

#include "AssetImporter.h"

namespace VWolfPup {
    class ModelImporter: public AssetImporter {
    public:
        uint32_t LoadPriority() override;
        bool Import(std::filesystem::path path, VWolf::UUID _id) override;
    protected:
        const std::vector<Extension> Extensions() const override;
    private:
        BOOST_DESCRIBE_CLASS(ModelImporter, (AssetImporter), (), (), ())
        VWOLF_SERIALIZATION_FRIENDS(ModelImporter)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolfPup::ModelImporter);
}
