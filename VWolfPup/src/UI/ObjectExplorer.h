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
#include <functional>

namespace VWolfPup {
    class ObjectExplorer: public View {
    public:
        ObjectExplorer(std::string extension, std::function<void(std::filesystem::path)> onSelection);
        ~ObjectExplorer();
    public:
        void OnGui() override;
    private:
        void FindObjects();
        void FindObjects(std::filesystem::path path);
    private:
        std::string extension;
        std::map<std::filesystem::path, std::string> files;
        std::function<void(std::filesystem::path)> onSelection;
        bool isOpen = true;
    };
}
