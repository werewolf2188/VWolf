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
#include <vector>
#include <map>

#include "VWolf.h"

#include <filewatch/FileWatch.hpp>

#include "../Camera/EditorCamera.h"

namespace VWolfPup {
    class Project {

    public:
        class EditorCamera {
        public:
            EditorCamera() = default;
        public:
            inline float GetPitch() { return m_Pitch; }
            inline float GetYaw() { return m_Yaw; }
            inline float GetDistance() { return m_Distance; }

            inline void SetPitch(float pitch) { m_Pitch = pitch; }
            inline void SetYaw(float yaw) { m_Yaw = yaw; }
            inline void SetDistance(float pitch) { m_Distance = pitch; }
    
        public:
            void SetCameraControllerInformation(VWolf::Ref<VWolfPup::CameraController> controller);
            void GetCameraControllerInformation(VWolf::Ref<VWolfPup::CameraController> controller);
        private:
            // For rotation
            float m_Pitch = 0.240f;
            float m_Yaw = -0.451f;
            // For zoom
            float m_Distance = 10.0f;
        };

        class Settings {
        public:
            Settings() = default;
            Settings(std::filesystem::path path);
            ~Settings();
        public:
            VWolf::DriverType GetType() { return type; }
            void SetType(VWolf::DriverType type) { this->type = type; }
            std::string GetCurrentSceneRelativePath() { return currentSceneRelativePath; }
            void SetCurrentSceneRelativePath(std::string relativePath) { this->currentSceneRelativePath = relativePath; }
            std::string GetProjectName() { return path.filename().string(); }
            
            EditorCamera& GetEditorCameraSettings() { return editorCameraSettings; }
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
            EditorCamera editorCameraSettings;
            std::string currentSceneRelativePath;
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
        VWolf::Ref<VWolf::Scene> GetCurrentScene();
        void LoadAssets();
        VWolf::Ref<VWolf::Material> GetMaterial(std::filesystem::path inPath);
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
        void LoadObjects(std::filesystem::path path, std::vector<std::filesystem::path>& sceneFiles);
    private:
        Settings settings;
        std::filesystem::path projectPath;
        filewatch::FileWatch<std::string>* watch;
        VWolf::Ref<VWolf::Scene> currentScene;
        std::map<std::uintptr_t, std::function<void(const std::string& path, const filewatch::Event event)>> _observers;

        std::map<std::filesystem::path, VWolf::Ref<VWolf::Material>> materials;
        std::map<std::filesystem::path, VWolf::Ref<VWolf::Scene>> scenes;
    private:
        static VWolf::Ref<Project> currentProject;
    };

    VWolf::DriverType LoadProject();
}
