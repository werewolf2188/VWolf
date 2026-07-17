//
//  AssetDatabase.hpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/3/26.
//

#pragma once
#include "AssetMetaFile.h"
#include <filesystem>

namespace VWolfPup {
    class AssetDatabase {
    public:
        // TODO: Remove
        static void CreateMetaFilesForEditor();
    public:
        static void LoadMetaFilesForEditor();
        static void CreateMetaFile(std::filesystem::path path);
        static void RemoveMetaFile(std::filesystem::path path);    
    private:
        static std::vector<AssetMetaFile> metafiles;
    };
}
