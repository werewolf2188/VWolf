//
//  FileTemplate.h
//  VWolfPup
//
//  Created by Enrique Moises on 7/14/26.
//

#pragma once
#include <filesystem>
#include <map>
#include "VWolf.h"

#include "Extensions.h"

namespace VWolfPup {
    class FileTemplate: public VWolf::Object {
    public:
        FileTemplate();
        FileTemplate(std::filesystem::path path, VWolf::UUID _id);
    public:
        std::filesystem::path GetPath() { return path; }
    public:
        static void Load(std::filesystem::path path, VWolf::UUID _id);
        static VWolf::Ref<FileTemplate> Find(const Extension& extension);
    private:
        std::filesystem::path path;
        
        static std::map<Extension, VWolf::Ref<FileTemplate>> fileTemplates;
    };
}
