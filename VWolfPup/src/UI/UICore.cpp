//
//  UICore.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#include "UICore.h"

#include <imgui/imgui.h>

namespace VWolfPup {

    void InitializeEditor() {
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    }

    View::View(std::string title): title(title) {}

    View::~View() {}

    MenuItem::MenuItem(std::string title, std::function<void(std::string title)> onPressed): View(title), onPressed(onPressed) {}

    MenuItem::~MenuItem() {}

    void MenuItem::OnGui() {
        ImGui::MenuItem(title.c_str(), nullptr, &currentValue);
        if (currentValue) {
            onPressed(title);
            currentValue = false;
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

    ContainerView::ContainerView(std::string title, std::initializer_list<View *> views): View(title), views(views) {}

    ContainerView::~ContainerView() {}

    void ContainerView::OnGui() {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    
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

        ImGui::PopStyleVar(2);

        ImGui::Begin(title.c_str(), nullptr, window_flags);
        ImGuiID dockspace_id = ImGui::GetID(title.c_str());
        
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        if (menuBar) {
            menuBar->OnGui();
        }
        ImGui::End();
    
        for (View* view: views) {
            view->OnGui();
        }
    }
}
