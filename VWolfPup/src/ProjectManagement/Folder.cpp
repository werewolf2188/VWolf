//
//  Folder.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/16/24.
//

#include "Folder.h"

namespace VWolfPup {
    Folder::Folder(std::string name, std::string folder): name(name), folder(folder) {}

    std::vector<Folder> Folder::folders = {
        Folder("Assets", "Assets")
    };

    std::string Folder::GetFolderName(std::string name) {
        return GetFolderByName(name).GetFolderName();
    }

    Folder Folder::GetFolderByName(std::string name) {
        return (*std::find_if(folders.begin(), folders.end(), [name](Folder folder) {
                return folder.GetName() == name;
            }));
    }

    Folder Folder::GetFolderByFolderName(std::string ext) {
        return (*std::find_if(folders.begin(), folders.end(), [ext](Folder folder) {
                return folder.GetFolderName() == ext;
            }));
    }
}
