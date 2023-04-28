//
//  FileBrowser.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/21/23.
//

#include "FileBrowser.h"

namespace VWolfPup {
    FileBrowser::FileBrowser(FileBrowserMode mode, std::function<void(std::filesystem::path)> onSelect):
    onSelect(onSelect), View("File Browser") {
        fileDialog = ImGui::FileBrowser(mode == FileBrowserMode::Save ?
                                        ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir :
                                        0);
        fileDialog.SetTitle("Test.scene");
        fileDialog.SetTypeFilters({ ".scene" });
    }
    FileBrowser::~FileBrowser() {
        
    }

    void FileBrowser::OnGui() {
        fileDialog.Display();
        if (fileDialog.HasSelected()) {
            onSelect(fileDialog.GetSelected());
            fileDialog.ClearSelected();
            isOpen = false;
        } else if (!fileDialog.IsOpened()) {
            isOpen = false;
        }
    }

    void FileBrowser::Open() {
        isOpen = true;
        fileDialog.Open();
    }

    bool FileBrowser::IsOpen() {
        return isOpen;
    }
}
