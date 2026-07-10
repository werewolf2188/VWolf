//
//  FileTemplate.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 7/14/26.
//

#include "FileTemplate.h"

namespace VWolfPup {

    std::map<Extension, VWolf::Ref<FileTemplate>> FileTemplate::fileTemplates;

    FileTemplate::FileTemplate(): VWolf::Object(VWolf::UUID::NewUUID()) {}

    FileTemplate::FileTemplate(std::filesystem::path path, VWolf::UUID _id): VWolf::Object(_id), path(path) {}

    void FileTemplate::Load(std::filesystem::path path, VWolf::UUID _id) {
        
        std::stringstream ss(path.stem().string());
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(ss, token, '.')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() < 2) return;
        
        VWolf::Ref<FileTemplate> fileTemplate = VWolf::CreateRef<FileTemplate>(path, _id);
        VWolf::ObjectResourceManager::AddObject(_id, fileTemplate);
        
        fileTemplates[Extension("." + tokens[1])] = fileTemplate;
    }

    VWolf::Ref<FileTemplate> FileTemplate::Find(const Extension& extension) {
        return fileTemplates[extension];
    }
}
