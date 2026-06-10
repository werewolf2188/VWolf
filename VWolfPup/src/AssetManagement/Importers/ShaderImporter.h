//
//  ShaderImporter.hpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/6/26.
//

#pragma once

#include "AssetImporter.h"

namespace VWolfPup {
    class ShaderImporter: public AssetImporter {
    public:
        uint32_t LoadPriority() override;
        bool Import(std::filesystem::path path, VWolf::UUID _id) override;
    protected:
        const std::vector<Extension> Extensions() const override;
    private:
        static Extension shaderExtension;
        static Extension shaderLibraryExtension;
        BOOST_DESCRIBE_CLASS(ShaderImporter, (AssetImporter), (), (), ())
        VWOLF_SERIALIZATION_FRIENDS(ShaderImporter)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolfPup::ShaderImporter);
}
