//
//  AudioImporter.h
//  VWolfPup
//
//  Created by Enrique Moises on 6/24/26.
//

#pragma once

#include "AssetImporter.h"

namespace VWolfPup {
    class AudioImporter: public AssetImporter {
    public:
        uint32_t LoadPriority(std::filesystem::path path) const override;
        bool Import(std::filesystem::path path, VWolf::UUID _id) override;
    protected:
        const std::vector<Extension> Extensions() const override;
    private:
        BOOST_DESCRIBE_CLASS(AudioImporter, (AssetImporter), (), (), ())
        VWOLF_SERIALIZATION_FRIENDS(AudioImporter)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolfPup::AudioImporter);
}
