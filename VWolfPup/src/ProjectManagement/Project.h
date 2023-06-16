//
//  Project.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#pragma once

#include <string>
#include <filesystem>
#include <functional>
#include <map>

#include "VWolf.h"

#include <filewatch/FileWatch.hpp>

namespace VWolfPup {
    class Project {
    public:
        class Settings {
        public:
            Settings() = default;
            Settings(std::filesystem::path path);
            ~Settings();
        public:
            VWolf::DriverType GetType() { return type; }
            void SetType(VWolf::DriverType type) { this->type = type; }
            std::string GetProjectName() { return path.filename(); }
        public:
            void Save();
            void Load();
        private:
#ifdef VWOLF_PLATFORM_WINDOWS
        VWolf::DriverType type = VWolf::DriverType::DirectX12;
#else
        VWolf::DriverType type = VWolf::DriverType::OpenGL;
#endif
            std::filesystem::path path;
        };
    public:
        Project(std::filesystem::path path);
        ~Project();
    public:
        VWolf::DriverType GetType() { return settings.GetType(); }
        Settings& GetSettings() { return settings; }
        std::filesystem::path GetProjectPath() { return projectPath; }
        std::filesystem::path GetProjectFileName() { return settings.GetProjectName(); }
    public:
        void AddObserver(std::uintptr_t, std::function<void(const std::string& path, const filewatch::Event event)>);
        void Save();
        std::filesystem::path GetAssetsPath();
    public:
        template<typename T>
        void AddObserver(T* obj, std::function<void(const std::string& path, const filewatch::Event event)> value) {
            std::uintptr_t key = reinterpret_cast<std::uintptr_t>(obj);
            this->AddObserver(key, value);
        }
    public:
        static void InitializeCurrentProject(VWolf::Ref<Project>);
        static VWolf::Ref<Project> CurrentProject();
    private:
        Settings settings;
        std::filesystem::path projectPath;
        filewatch::FileWatch<std::string>* watch;
        std::map<std::uintptr_t, std::function<void(const std::string& path, const filewatch::Event event)>> _observers;
    private:
        static VWolf::Ref<Project> currentProject;
    };

    VWolf::DriverType LoadProject();
}
