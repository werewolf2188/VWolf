//
//  LoadSettings.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 8/13/23.
//

#include "LoadSettings.h"

#include <filesystem>
#include <vector>

#include "../ProjectManagement/Project.h"
#include "../ProjectManagement/Extensions.h"
#include "../ProjectManagement/Folder.h"

#include <yaml-cpp/yaml.h>
#include "Serialization/DefaultSettings.h"
#include "AssetManagement/AssetMetaFile.h"

namespace VWolfPup {

    // MARK: Defaults
    VWolf::Ref<Defaults> Defaults::defaults;

    Defaults::Defaults(Defaults& defaults) {
        this->defaultMaterial = defaults.defaultMaterial;
        this->defaultGridMaterial = defaults.defaultGridMaterial;
        this->defaultSkyBoxMaterial = defaults.defaultSkyBoxMaterial;
    }

    bool Defaults::IsDefault(VWolf::Material& material) {
        for(auto defaultMaterial: materials) {
            if (defaultMaterial.second->GetName() == material.GetName()) {
                return true;
            }
        }
        return false;
    }

    void Defaults::Load() {
        constexpr const char * fileName = "defaults.ini";
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(fileName);
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CLIENT_ERROR("Failed to load .scene file '%s'\n     %s", fileName, e.what());
        }

        Defaults defaults = data.as<Defaults>();
        Defaults::defaults = VWolf::CreateRef<Defaults>(defaults);
    }
}
