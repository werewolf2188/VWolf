//
//  AssetImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/6/26.
//

#include "AssetImporter.h"

namespace VWolfPup {
    const bool AssetImporter::CanImport(std::filesystem::path path) const {
        
        if (std::filesystem::is_directory(path)) return false;
        
        Extension extension = path;
        const std::vector<Extension>& extensions = Extensions();
        
        return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
    }
}
