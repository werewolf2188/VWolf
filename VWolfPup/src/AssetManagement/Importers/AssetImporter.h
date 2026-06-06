//
//  AssetImporter.hpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/6/26.
//

#pragma once
#include <filesystem>
#include <algorithm>

#include "VWolf.h"
#include "../ProjectManagement/Extensions.h"

namespace VWolfPup {
    class AssetImporter {
    public:
        virtual uint32_t LoadPriority() { return -1; };
    public:
        virtual const bool CanImport(std::filesystem::path path) const;
    protected:
        virtual const std::vector<Extension> Extensions() const { return {}; };
    private:
        BOOST_DESCRIBE_CLASS(AssetImporter, (), (), (), ())
    };
}
