//
//  Project.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/20/23.
//

#pragma once

enum class ProjectObjectsConstantKeys {
    Project, ProjectName, ProjectDriver, ProjectEditorCamera, EditorCameraYaw, EditorCameraPitch, EditorCameraDistance, ProjectCurrentScene, SceneRelativePath
};

static std::map<ProjectObjectsConstantKeys, const char*> projectKeys = {
    { ProjectObjectsConstantKeys::Project, "Project" },
    { ProjectObjectsConstantKeys::ProjectName, "Name" },
    { ProjectObjectsConstantKeys::ProjectDriver, "Driver" },
    { ProjectObjectsConstantKeys::ProjectEditorCamera, "EditorCamera" },
    { ProjectObjectsConstantKeys::EditorCameraYaw, "Yaw" },
    { ProjectObjectsConstantKeys::EditorCameraPitch, "Pitch" },
    { ProjectObjectsConstantKeys::EditorCameraDistance, "Distance" },
    { ProjectObjectsConstantKeys::ProjectCurrentScene, "CurrentScene" },
    { ProjectObjectsConstantKeys::SceneRelativePath, "RelativePath" }
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

            if (node[projectKeys[ProjectObjectsConstantKeys::ProjectEditorCamera]]) {
                auto editorCameraNode = node[projectKeys[ProjectObjectsConstantKeys::ProjectEditorCamera]];

                if (editorCameraNode[projectKeys[ProjectObjectsConstantKeys::EditorCameraYaw]]) {
                    rhs.GetEditorCameraSettings().SetYaw(editorCameraNode[projectKeys[ProjectObjectsConstantKeys::EditorCameraYaw]].as<float>());
                }

                if (editorCameraNode[projectKeys[ProjectObjectsConstantKeys::EditorCameraPitch]]) {
                    rhs.GetEditorCameraSettings().SetPitch(editorCameraNode[projectKeys[ProjectObjectsConstantKeys::EditorCameraPitch]].as<float>());
                }

                if (editorCameraNode[projectKeys[ProjectObjectsConstantKeys::EditorCameraDistance]]) {
                    rhs.GetEditorCameraSettings().SetDistance(editorCameraNode[projectKeys[ProjectObjectsConstantKeys::EditorCameraDistance]].as<float>());
                }
            }

            if (node[projectKeys[ProjectObjectsConstantKeys::ProjectCurrentScene]]) {
                auto currentScene = node[projectKeys[ProjectObjectsConstantKeys::ProjectCurrentScene]];
                if (currentScene[projectKeys[ProjectObjectsConstantKeys::SceneRelativePath]]) {
                    rhs.SetCurrentSceneRelativePath(currentScene[projectKeys[ProjectObjectsConstantKeys::SceneRelativePath]].as<std::string>());
                }
            }
            
            VWOLF_CLIENT_DEBUG("Decoding Pitch (%.3f), Yaw (%.3f), Distance (%.3f)",
                               rhs.GetEditorCameraSettings().GetPitch(),
                               rhs.GetEditorCameraSettings().GetYaw(),
                               rhs.GetEditorCameraSettings().GetDistance());
            
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
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::ProjectEditorCamera];
        out << YAML::BeginMap;
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::EditorCameraYaw] << YAML::Value << v.GetEditorCameraSettings().GetYaw();
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::EditorCameraPitch] << YAML::Value << v.GetEditorCameraSettings().GetPitch();
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::EditorCameraDistance] << YAML::Value << v.GetEditorCameraSettings().GetDistance();
        out << YAML::EndMap;
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::ProjectCurrentScene];
        out << YAML::BeginMap;
        out << YAML::Key << projectKeys[ProjectObjectsConstantKeys::SceneRelativePath] << YAML::Value << v.GetCurrentSceneRelativePath();
        out << YAML::EndMap;
        out << YAML::EndMap;
        out << YAML::EndMap;
        return out;
    }
}
