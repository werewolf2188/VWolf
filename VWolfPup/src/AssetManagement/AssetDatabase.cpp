//
//  AssetDatabase.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/3/26.
//

#include "AssetDatabase.h"

#include <string>
#include <algorithm>

#include "VWolf.h"

#include "../ProjectManagement/Project.h"

namespace VWolfPup {
    std::vector<AssetMetaFile> AssetDatabase::metafiles;

    void SortMetafiles(std::vector<AssetMetaFile>& metafiles) {
        std::sort(metafiles.begin(), metafiles.end(), [](const AssetMetaFile& fileA, const AssetMetaFile& fileB) {
            return fileA.LoadPriority() < fileB.LoadPriority();
        });
    }

    void AssetDatabase::CreateMetaFile(std::filesystem::path entry) {
        AssetMetaFile mf(entry);
        
        if (mf.Create()) {
            metafiles.push_back(mf);
            // Sorting
            SortMetafiles(metafiles);
            mf.Import();
        }
    }

    void AssetDatabase::RemoveMetaFile(std::filesystem::path path) {
        AssetMetaFile mf(path);
        
        if (mf.Remove()) {
            metafiles.erase(std::remove_if(metafiles.begin(), metafiles.end(), [mf](const AssetMetaFile& x) {
                return mf == x;
            }), metafiles.end());
            SortMetafiles(metafiles);
        }
    }

    void AssetDatabase::CreateMetaFilesForEditor() {
        // vwolfmat C:\Users\jenri\Projects\Desktop\VWolf\bin\Debug-windows-x86_64\VWolfPup
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        std::filesystem::path editorAssetsFolder = "/Users/enriquemoises/Projects/Personal/VWolf/VWolfPup/src/assets/";
        std::filesystem::path editorShadersFolder = "/Users/enriquemoises/Projects/Personal/VWolf/VWolfPup/src/shaders/";
#elif defined(VWOLF_PLATFORM_WINDOWS) 
        std::filesystem::path editorAssetsFolder = "C:\\Users\\jenri\\Projects\\Desktop\\VWolf\\VWolfPup\\src\\assets";
		std::filesystem::path editorShadersFolder = "C:\\Users\\jenri\\Projects\\Desktop\\VWolf\\VWolfPup\\src\\shaders";
#endif

        for (const auto& entry : std::filesystem::recursive_directory_iterator(editorAssetsFolder)) {
            AssetMetaFile mf(entry);
            mf.Create();
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(editorShadersFolder)) {
            AssetMetaFile mf(entry);
            mf.Create();
        }
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(Project::CurrentProject()->GetAssetsPath())) {
            AssetMetaFile mf(entry);
            mf.Create();
        }
        
        for (const AssetMetaFile& currentAssetMetafile: metafiles) {
            currentAssetMetafile.Save();
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
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(Project::CurrentProject()->GetAssetsPath())) {
            if (AssetMetaFile::IsMetafile(entry)) {
                metafiles.push_back(AssetMetaFile::Load(entry));
            }
        }
        // Sorting
        SortMetafiles(metafiles);
//        std::cout << "End" << std::endl;
        std::for_each(metafiles.begin(), metafiles.end(), [](AssetMetaFile& file) {
            file.Import();
        });
//        std::cout << "End" << std::endl;
    }
}
