//
//  DefaultSettings.h
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/12/24.
//

#pragma once

enum class DefaultSettingsConstantKeys {
    Defaults, DefaultMaterial, DefaultSkyBoxMaterial, DefaultGridMaterial
};

static std::map<DefaultSettingsConstantKeys, const char*> defaultSettingsKeys = {
    { DefaultSettingsConstantKeys::Defaults, "Defaults" },
    { DefaultSettingsConstantKeys::DefaultMaterial, "Render Material" },
    { DefaultSettingsConstantKeys::DefaultSkyBoxMaterial, "Skybox Material" },
    { DefaultSettingsConstantKeys::DefaultGridMaterial, "Grid Material" }
};

namespace YAML {
    template<>
    struct convert<VWolfPup::Defaults>
    {
        static bool decode(const Node& node, VWolfPup::Defaults& rhs)
        {
            if (!node.IsMap() ||
                !node[defaultSettingsKeys[DefaultSettingsConstantKeys::Defaults]])
                return false;
    
            auto defaults = node[defaultSettingsKeys[DefaultSettingsConstantKeys::Defaults]];

            if (defaults[defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultMaterial]]) {
                rhs.SetDefaultMaterialName(defaults[defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultMaterial]].as<std::string>());
            }
            if (defaults[defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultSkyBoxMaterial]]) {
                rhs.SetDefaultSkyBoxMaterialName(defaults[defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultSkyBoxMaterial]].as<std::string>());
            }
            if (defaults[defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultGridMaterial]]) {
                rhs.SetDefaultGridMaterialName(defaults[defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultGridMaterial]].as<std::string>());
            }

            return true;
        }
    };
}

namespace VWolfPup {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolfPup::Defaults& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << defaultSettingsKeys[DefaultSettingsConstantKeys::Defaults];
        out << YAML::BeginMap;
        out << YAML::Key << defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultMaterial] << YAML::Value << v.GetDefaultMaterialName();
        out << YAML::Key << defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultSkyBoxMaterial] << YAML::Value << v.GetDefaultSkyBoxMaterialName();
        out << YAML::Key << defaultSettingsKeys[DefaultSettingsConstantKeys::DefaultGridMaterial] << YAML::Value << v.GetDefaultGridMaterialName();
        out << YAML::EndMap;
        out << YAML::EndMap;
        return out;
    }
}
