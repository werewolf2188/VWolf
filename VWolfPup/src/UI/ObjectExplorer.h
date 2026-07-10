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

    template<class T>
    class ObjectExplorer: public View {
    public:
        ObjectExplorer(std::function<void(VWolf::Ref<T>)> onSelection, std::function<VWolf::Ref<T>()> onNoneSelection):
        View(VWolf::ClassNameCleaner::Current().GetClassName<T>()), onSelection(onSelection), onNoneSelection(onNoneSelection),
        objects(VWolf::ObjectResourceManager::Filter<T>()) {}
        ~ObjectExplorer() {}
    public:
        void OnGui() override {
            if (!isOpen) {
                ContainerView::GetMainView()->RemoveView(this);
            }
                
            ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoSavedSettings);
            if (ImGui::Selectable("None")) {
                onSelection(onNoneSelection());
                isOpen = false;
            }
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
        std::function<VWolf::Ref<T>()> onNoneSelection;
        bool isOpen = true;
    };
}
