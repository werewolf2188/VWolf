//
//  AssetDatabase.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/3/26.
//

#include "AssetDatabase.h"

#include <string>

#include "VWolf.h"

#include "../ProjectManagement/Project.h"

namespace VWolfPup {
    std::vector<AssetMetaFile> AssetDatabase::metafiles;

    void AssetDatabase::CreateMetaFile(std::filesystem::path entry) {
        AssetMetaFile mf(entry);
        
        mf.Create();
    }

    void AssetDatabase::CreateMetaFilesForEditor() {
        // vwolfmat
        std::filesystem::path editorAssetsFolder = "/Users/enriquemoises/Projects/Personal/VWolf/VWolfPup/src/assets/";
        std::filesystem::path editorShadersFolder = "/Users/enriquemoises/Projects/Personal/VWolf/VWolfPup/src/shaders/";

        for (const auto& entry : std::filesystem::recursive_directory_iterator(editorAssetsFolder)) {
            CreateMetaFile(entry);
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(editorShadersFolder)) {
            CreateMetaFile(entry);
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(Project::CurrentProject()->GetAssetsPath())) {
            CreateMetaFile(entry);
        }
    }

    void AssetDatabase::LoadMetaFilesForEditor() {
        std::filesystem::path currentExecutable = VWolf::CommandLineArguments::GetArguments()[0];
        std::filesystem::path currentFolder = currentExecutable.parent_path();
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(currentFolder)) {
            if (AssetMetaFile::IsMetafile(entry)) {
                metafiles.push_back(AssetMetaFile::Load(entry));
            }
        }
    }
}
