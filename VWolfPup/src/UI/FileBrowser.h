//
//  FileBrowser.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/21/23.
//

#include "UICore.h"
#include "VWolf.h"
#include <functional>
#include <imgui/imgui.h>
#include "FileDialog/imfilebrowser.h"

namespace VWolfPup {
    enum class FileBrowserMode {
        Save, Open
    };
    class FileBrowser: public View {
    public:
        FileBrowser(FileBrowserMode mode, std::function<void(std::filesystem::path)> onSelect);
        ~FileBrowser();
    public:
        void OnGui() override;
        void Open();
    private:
        ImGui::FileBrowser fileDialog;
        std::function<void(std::filesystem::path)> onSelect;
    };
}
