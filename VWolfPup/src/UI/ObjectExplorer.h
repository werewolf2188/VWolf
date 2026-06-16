//
//  ObjectExplorer.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/22/23.
//

#pragma once

#include "UICore.h"
#include "VWolf.h"

#include <filesystem>
#include <map>
#include <functional>

#include <imgui/imgui.h>

namespace VWolfPup {
    class FileExplorer: public View {
    public:
        FileExplorer(std::string extension, std::function<void(std::filesystem::path)> onSelection);
        ~FileExplorer();
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

    template<class T>
    class ObjectExplorer: public View {
    public:
        ObjectExplorer(std::function<void(VWolf::Ref<T>)> onSelection): View(VWolf::ClassNameCleaner::Current().GetClassName<T>()),
        onSelection(onSelection), objects(VWolf::ObjectResourceManager::Filter<T>()) {}
        ~ObjectExplorer() {}
    public:
        void OnGui() override {
            if (!isOpen) {
                ContainerView::GetMainView()->RemoveView(this);
            }
                
            ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoSavedSettings);
            for (auto entry: objects) {
                if (ImGui::Selectable(entry->GetName().c_str())) {
                    onSelection(entry);
                    isOpen = false;
                }
            }
            ImGui::End();
        }
    private:
        std::vector<VWolf::Ref<T>> objects;
        std::function<void(VWolf::Ref<T>)> onSelection;
        bool isOpen = true;
    };
}
