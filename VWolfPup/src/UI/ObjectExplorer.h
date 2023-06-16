//
//  ObjectExplorer.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/22/23.
//

#pragma once

#include "UICore.h"

#include <filesystem>
#include <map>

namespace VWolfPup {
    class ObjectExplorer: public View {
    public:
        ObjectExplorer(std::string extension);
        ~ObjectExplorer();
    public:
        void OnGui() override;
    private:
        void FindObjects();
        void FindObjects(std::filesystem::path path);
    private:
        std::string extension;
        std::map<std::filesystem::path, std::string> files;
        bool isOpen = true;
    };
}
