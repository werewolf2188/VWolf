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

    class ContainerView;

    class View {
    public:
        View(std::string title);
        ~View();
    public:
        virtual void OnGui() = 0;
        
    public:
        View* GetParent() { return parent; }
        ContainerView* GetContainer() { return container; }
        std::string& GetTitle() { return title; }
    protected:
        std::string title;
    protected:
        virtual void SetInContainer() { }
        virtual void AfterSetInContainer() { }
    private:
        View* parent;
        ContainerView* container;
        
        friend class ContainerView;
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

    struct ContainerTree {
    private:
        ContainerTree(unsigned int splitId, int side, ContainerTree *parent = nullptr);
    public:
        ContainerTree* Create(int side);
        ContainerTree* Get(int side);
        bool Contains(int side);
        void Install(View * view);
        void Install(View * view, int side);
    public:
        bool operator==(ContainerTree& tree);
    private:
        ContainerTree* parent = nullptr;
        unsigned int splitId = 0;
        int side = 0;
        std::vector<ContainerTree*> children;

        friend class ContainerView;
    };

    class ContainerView: public View {
    public:
        ContainerView(std::string title, std::initializer_list<View *> views);
        ~ContainerView();
    public:
        void SetMenuBar(MenuBar* menuBar) {
            this->menuBar = menuBar;
            SetContainerAndParent(menuBar);
        };
        void AddView(View* view) {
            views.push_back(view);
            SetContainerAndParent(view);
        }
        bool IsInitializing() { return initializing; }
        ContainerTree* GetRoot() { return root; }
    public:
        void SaveIniFile();
    public:
        void OnGui() override;
    public:
        static ContainerView* GetMainView();
    private:
        void SetContainerAndParent(View* view) {
            view->parent = this;
            view->container = this;
        }
        void LoadIniFile();
    private:
        std::vector<View *> views;
        MenuBar* menuBar;
        unsigned int dockspace_id;
        bool initializing = true;
        bool starting = true;
        const char *dockName;
        ContainerTree* root;
    private:
        static ContainerView* mainView;
    };
}
