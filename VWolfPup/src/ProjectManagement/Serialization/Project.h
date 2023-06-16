//
//  Project.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/20/23.
//

#pragma once

enum class ProjectObjectsConstantKeys {
    Project, ProjectName, ProjectDriver
};

static std::map<ProjectObjectsConstantKeys, const char*> projectKeys = {
    { ProjectObjectsConstantKeys::Project, "Project" },
    { ProjectObjectsConstantKeys::ProjectName, "Name" },
    { ProjectObjectsConstantKeys::ProjectDriver, "Driver" }
};

namespace YAML {
    template<>
    struct convert<VWolfPup::Project::Settings>
    {
        static bool decode(const Node& node, VWolfPup::Project::Settings& rhs)
        {
            if (!node.IsMap() ||
                !node[projectKeys[ProjectObjectsConstantKeys::ProjectName]])
                return false;

            if (node[projectKeys[ProjectObjectsConstantKeys::ProjectDriver]]) {
                std::string driver = node[projectKeys[ProjectObjectsConstantKeys::ProjectDriver]]
                    .as<std::string>();
                rhs.SetType(VWolf::GetDriverType(driver.c_str()));
            }
            
            return true;
        }
    };
}

namespace VWolfPup {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolfPup::Project::Settings& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::Project];
        out << YAML::BeginMap;
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::ProjectName] << YAML::Value << v.GetProjectName();
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::ProjectDriver] << YAML::Value << VWolf::DriverName(v.GetType());
        out << YAML::EndMap;
        out << YAML::EndMap;
        return out;
    }
}
