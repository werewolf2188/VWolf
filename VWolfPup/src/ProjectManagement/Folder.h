//
//  Folder.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/16/24.
//

#pragma once

#include <string>
#include <vector>

namespace VWolfPup {
    class Folder{
    public:
        Folder() = default;
        Folder(std::string name, std::string folder);
    public:
        inline std::string GetName() { return name; }
        inline std::string GetFolderName() { return folder; }
    public:
        static Folder GetFolderByName(std::string name);
        static Folder GetFolderByFolderName(std::string ext);
        static std::string GetFolderName(std::string name);
    public:
        static std::string GetAssetsFolder() { return GetFolderName("Assets"); }
    private:
        std::string name;
        std::string folder;
    private:
        static std::vector<Folder> folders;
    };
}
