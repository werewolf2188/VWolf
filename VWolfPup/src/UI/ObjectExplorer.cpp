//
//  ObjectExplorer.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/22/23.
//

#include "ObjectExplorer.h"

#include "../ProjectManagement/Project.h"

#include <imgui/imgui.h>

namespace VWolfPup {
    static const std::string MAIN_STATIC_ASSETS_LIBRARY = "assets";

    ObjectExplorer::ObjectExplorer(std::string extension):
    View(std::string("List of .") + extension + std::string(" files")), extension(extension) {
        FindObjects();
    }

    ObjectExplorer::~ObjectExplorer() {
        
    }

    void ObjectExplorer::OnGui() {
        if (!isOpen) {
            ContainerView::GetMainView()->RemoveView(this);
        };

        ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoSavedSettings);
        for (auto entry: files) {
            if (ImGui::Selectable(entry.second.c_str())) {
                VWOLF_CLIENT_INFO("Selected: %s", entry.second.c_str());
            }
        }
        ImGui::End();
    }

    void ObjectExplorer::FindObjects() {
        // We find objects in the main assets directory
        std::filesystem::path mainAssetsPath = std::filesystem::current_path() / MAIN_STATIC_ASSETS_LIBRARY;
        FindObjects(mainAssetsPath);
        // We fine objects in the project assets directory
        std::filesystem::path projectAssetsPath = Project::CurrentProject()->GetAssetsPath();
        FindObjects(projectAssetsPath);
    }

    void ObjectExplorer::FindObjects(std::filesystem::path path) {
        std::filesystem::directory_entry entry(path);
        if (entry.is_directory()) {
            for (auto entry: std::filesystem::directory_iterator(path)) {
                FindObjects(entry.path());
            }
            // Is directory
        } else if (path.extension().string() == extension) {
            // has extension
            files[path] = path.filename().string();
        }
    }
}
