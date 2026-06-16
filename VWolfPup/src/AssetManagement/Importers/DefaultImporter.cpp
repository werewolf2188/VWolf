//
//  DefaultImporter.cpp
//  VWolfPup
//
//  Created by Enrique Moises on 6/7/26.
//

#include "DefaultImporter.h"
#include "../ProjectManagement/Project.h"


namespace VWolfPup {
    Extension DefaultImporter::sceneExtension("Scene", ".scene");

    uint32_t DefaultImporter::LoadPriority(std::filesystem::path path) const {
        return std::numeric_limits<uint32_t>::max();
    }

    const std::vector<Extension> DefaultImporter::Extensions() const {
        return {
            sceneExtension
        };
    }

    bool DefaultImporter::Import(std::filesystem::path path, VWolf::UUID _id) {
        if (std::filesystem::is_directory(path)) return true;
        
        if (sceneExtension == path) {
            VWolf::Ref<VWolf::Scene> scene = VWolf::Scene::Load(path, _id);
            Project::CurrentProject()->AddScene(path, scene);
        }
        return true;
    }

    const bool DefaultImporter::CanImport(std::filesystem::path path) const {
        if (std::filesystem::is_directory(path)) return true;
        
        Extension extension = path;
        const std::vector<Extension>& extensions = Extensions();
        
        return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(DefaultImporter);
}
