//
//  UICore.h
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#pragma once

#include <string>
#include <vector>
#include <functional>

//#include "VWolf.h" // If I want to use shared pointer

namespace VWolfPup {

    void InitializeEditor();

    class View {
    public:
        View(std::string title);
        ~View();
    public:
        virtual void OnGui() = 0;
    protected:
        std::string title;
    };

    class MenuItem: public View {
    public:
        MenuItem();
        MenuItem(std::string title, std::function<void(std::string title)> onPressed);
        ~MenuItem();
    public:
        void OnGui() override;
    protected:
        bool currentValue = false;
        bool isSeparator = false;
        std::function<void(std::string title)> onPressed;
    };

    class Menu: public View {
    public:
        Menu(std::string title, std::initializer_list<MenuItem *> menuItems);
        ~Menu();
    public:
        void OnGui() override;
    private:
        std::vector<MenuItem *> menuItems;
    };

    class MenuBar: public View {
    public:
        MenuBar(std::string title, std::initializer_list<Menu *> menus);
        ~MenuBar();
    public:
        void OnGui() override;
    private:
        std::vector<Menu *> menus;
    };

    class ContainerView: public View {
    public:
        ContainerView(std::string title, std::initializer_list<View *> views);
        ~ContainerView();
    public:
        void SetMenuBar(MenuBar* menuBar) { this->menuBar = menuBar; };
        void AddView(View* view) { views.push_back(view); }
    public:
        void OnGui() override;
    private:
        std::vector<View *> views;
        MenuBar* menuBar;
    };
}
