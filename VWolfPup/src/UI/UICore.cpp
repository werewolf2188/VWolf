//
//  UICore.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#include <cstring>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <fstream>

#include "UICore.h"

#include "VWolf.h"

namespace VWolfPup {

    void InitializeEditor() {
        
        std::vector<std::string> s = VWolf::CommandLineArguments::GetArguments();

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        std::string folder = "assets/Open_Sans/static/";
        
        float fontSize = 18.0f;// *2.0f;
        io.Fonts->AddFontFromFileTTF((folder + std::string("OpenSans-Bold.ttf")).c_str(), fontSize);
        io.FontDefault = io.Fonts->AddFontFromFileTTF((folder + std::string("OpenSans-Regular.ttf")).c_str(), fontSize);
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Headers
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        
        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }

    View::View(std::string title): title(title) {}

    View::~View() {}

    MenuItem::MenuItem(): View("Separator"), isSeparator(true) {}
    MenuItem::MenuItem(std::string title, std::function<void(std::string title)> onPressed): View(title), onPressed(onPressed) {}

    MenuItem::~MenuItem() {}

    void MenuItem::OnGui() {
        if (isSeparator) {
            ImGui::Separator();
        }
        else {
            ImGui::MenuItem(title.c_str(), nullptr, &currentValue);
            if (currentValue) {
                onPressed(title);
                currentValue = false;
            }
        }
    }

    Menu::Menu(std::string title, std::initializer_list<MenuItem *> menuItems): View(title), menuItems(menuItems) {}

    Menu::~Menu() {}

    void Menu::OnGui() {
        if (ImGui::BeginMenu(title.c_str()))
        {
            for(MenuItem* menuItem: menuItems) {
                menuItem->OnGui();
            }
            ImGui::EndMenu();
        }
    }

    MenuBar::MenuBar(std::string title, std::initializer_list<Menu *> menus): View(title), menus(menus) {}

    MenuBar::~MenuBar() {}

    void MenuBar::OnGui() {
        if (ImGui::BeginMenuBar()) {
            for(Menu* menu: menus) {
                menu->OnGui();
            }
            ImGui::EndMenuBar();
        }
    }

// ---------------------
    ContainerTree:: ContainerTree(unsigned int splitId, int side, ContainerTree *parent):
    splitId(splitId), side(side), parent(parent) { }

    ContainerTree* ContainerTree::Create(int side) {
        ImGuiID id = ImGui::DockBuilderSplitNode(this->splitId, side, 0.25f, nullptr, &this->splitId);
        ContainerTree* child = new ContainerTree(id, side, this);
        children.push_back(child);
        return child;
    }

    ContainerTree* ContainerTree::Get(int side) {
        if (Contains(side)) {
            auto finder = [side](ContainerTree* child) {
                return child->side == side;
            };
            auto item = std::find_if(children.begin(), children.end(), finder);
            return *item;
        }
        return nullptr;
    }

    bool ContainerTree::Contains(int side) {
        auto finder = [side](ContainerTree* child) {
            return child->side == side;
        };
        auto item = std::find_if(children.begin(), children.end(), finder);
        return item != children.end();
    }

    void ContainerTree::Install(View * view) {
        ImGui::DockBuilderDockWindow(view->GetTitle().c_str(), splitId);
    }

    void ContainerTree::Install(View * view, int side) {
        ContainerTree* child;
        if ((child = Get(side))) {
            ImGui::DockBuilderDockWindow(view->GetTitle().c_str(), child->splitId);
        } else {
            child = Create(side);
            child->Install(view);
        }
    }

    bool ContainerTree::operator==(ContainerTree& tree) {
        return this->splitId == tree.splitId && this->side == tree.side;
    }
// ------------

    ContainerView* ContainerView::mainView = nullptr;

    ContainerView* ContainerView::GetMainView() {
        return mainView;
    }

    ContainerView::ContainerView(std::string title, std::initializer_list<View *> views):
    View(title), dockName("DockContainer"), views(views) {
        if (!mainView) {
            mainView = this;
        }
        LoadIniFile();
    }

    void ContainerView::LoadIniFile() {
        auto directory = std::filesystem::current_path().string();
        auto file = directory + std::string("/imgui.ini");
        
        std::fstream iniFile;
        iniFile.open(file.c_str());
        if (iniFile.good()) {
            ImGui::LoadIniSettingsFromDisk(file.c_str());
            VWOLF_CLIENT_INFO("Ini file loaded at %s", file.c_str());
            initializing = false;
        }
    }

    void ContainerView::SaveIniFile() {
        auto directory = std::filesystem::current_path().string();
        auto file = directory + std::string("/imgui.ini");
        
        ImGui::SaveIniSettingsToDisk(file.c_str());
        VWOLF_CLIENT_INFO("Ini file saved at %s", file.c_str());
    }

    ContainerView::~ContainerView() {}

    void ContainerView::RemoveView(View* view) {
        auto foundView = std::find(views.begin(), views.end(), view);
        views.erase(foundView);
    }

    void ContainerView::OnGui() {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(title.c_str(), nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);
        
        if (menuBar) {
            menuBar->OnGui();
        }
        // Create dockspace
        dockspace_id = ImGui::GetID(dockName);

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        ImGui::End();
        
        if (initializing) {
            root = new ContainerTree((unsigned int)dockspace_id, ImGuiDir_None);

            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
            
            for (View* view: views) {
                view->SetInContainer();
            }
        }

        uint32_t total = (uint32_t)views.size();
        for (uint32_t index = 0; index < total; index++) {
            views[index]->OnGui();
        }
        
        if (starting) {
            for (View* view: views) {
                view->AfterSetInContainer();
            }
            starting = false;
        }
        if (initializing)
            initializing = false;
    }
}
