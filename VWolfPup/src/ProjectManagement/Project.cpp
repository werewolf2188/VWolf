//
//  Project.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#include "Project.h"
#include "Extensions.h"
#include "Folder.h"

#include <yaml-cpp/yaml.h>

#include "Serialization/Project.h"

namespace VWolfPup {

    void SetupProject(std::filesystem::path& path) {
        std::filesystem::create_directory(path);

        std::filesystem::path assetsPath = path;
        assetsPath /= Folder::GetAssetsFolder();
        std::filesystem::create_directory(assetsPath);
    
        std::array<std::string, 2> subfolders = { "Scenes", "Materials" };
        for (std::string subFolder: subfolders) {
            std::filesystem::create_directory(assetsPath / subFolder);
        }
    }

    bool ProjectExists(std::filesystem::path& path) {
        std::string fileName = path.filename().string() + Extension::GetProjectExtension();
        return std::filesystem::exists(path) && std::filesystem::exists(path / Folder::GetAssetsFolder()) && std::filesystem::exists(path / fileName);
    }

    VWolf::DriverType LoadProject() {
        /*
         1. Detect if the folder and project file exists
         1.1. If it exists, load project
            1.1.1. Load project and project settings
         1.2. If it does not exists
            1.2.1. Create folder
            1.2.2. Create sub folders
            1.2.3. Create Project settings
         */
//        std::string projectLocation = VWolf::CommandLineArguments::GetArguments()[1];
#ifdef VWOLF_PLATFORM_WINDOWS   
        std::string projectLocation = "C:\\Users\\jenri\\Projects\\Desktop\\Game_Project";
#else 
        std::string projectLocation = "/Users/vn50tzy/Projects/Personal/Game_Project";
#endif
        
        std::filesystem::path path(projectLocation);
        if (!ProjectExists(path)) {
            SetupProject(path);
        }

        Project::InitializeCurrentProject(VWolf::CreateRef<Project>(projectLocation));
        return Project::CurrentProject()->GetType();
    }

    Project::Settings::Settings(std::filesystem::path path): path(path) {
        if (!std::filesystem::exists(path)) {
            Save();
        } else {
            Load();
        }
    }

    Project::Settings::~Settings() {
        
    }

    void Project::EditorCamera::SetCameraControllerInformation(VWolf::Ref<VWolfPup::CameraController> controller) {
        VWOLF_CLIENT_DEBUG("Pitch (%.3f), Yaw (%.3f), Distance (%.3f)", m_Pitch, m_Yaw, m_Distance);
        controller->SetPitch(m_Pitch);
        controller->SetYaw(m_Yaw);
        controller->SetDistance(m_Distance);
    }

    void Project::EditorCamera::GetCameraControllerInformation(VWolf::Ref<VWolfPup::CameraController> controller) {
        m_Pitch = controller->GetPitch();
        m_Yaw = controller->GetYaw();
        m_Distance = controller->GetDistance();
    }

    void Project::Settings::Save() {
        YAML::Emitter out;
        out << *this;
        std::ofstream fout(path.string());
        fout << out.c_str();
    }

    VWolf::Ref<VWolf::Scene> Project::GetCurrentScene() {
        return currentScene;
    }

    void Project::Settings::Load() {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CLIENT_ERROR("Failed to load .project settings file '%s'\n     %s", path.string().c_str(), e.what());
            throw std::filesystem::filesystem_error("Failed to load .project settings file", path, std::error_code());
        }

        if (!data[projectKeys[ProjectObjectsConstantKeys::Project]])
            return;
        
        Settings settings = data[projectKeys[ProjectObjectsConstantKeys::Project]].as<Settings>();
        // TODO: Should do a copy constructor
        this->type = settings.type;
        this->editorCameraSettings = settings.editorCameraSettings;
        this->currentSceneRelativePath = settings.currentSceneRelativePath;
    }

    Project::Project(std::filesystem::path path):
    projectPath(path),
    settings((path / path.filename()).concat(Extension::GetProjectExtension())) {
        watch = new filewatch::FileWatch<std::string> {
            path.string(),
            [this] (const std::string& path, const filewatch::Event event) {
                for (const auto& kv : _observers) {
                    kv.second(path, event);
                }
            }
        };
    }

    void Project::LoadAssets() {
        std::vector<std::filesystem::path> sceneFiles;
        LoadObjects(GetAssetsPath(), sceneFiles);
        for(auto const& sceneFile : sceneFiles) {
            VWolf::Ref<VWolf::Scene> scene = VWolf::SceneSerializer::Deserialize(sceneFile);
            auto path = GetAssetsPath() / settings.GetCurrentSceneRelativePath();
            if (path ==sceneFile)
                currentScene = scene;
            scenes[sceneFile] = scene;
        }
    }

    void Project::LoadObjects(std::filesystem::path path, std::vector<std::filesystem::path>& sceneFiles) {
        for(auto const& dir_entry : std::filesystem::directory_iterator(path)) {
            if (dir_entry.is_directory()){
                LoadObjects(dir_entry.path(), sceneFiles);
            } else if (Extension::HasExtension(dir_entry.path().extension().string())) {
                if (Extension::GetSceneExtension() == dir_entry.path().extension()) {
                    sceneFiles.push_back(dir_entry.path());
                } else  if (Extension::GetMaterialExtension() == dir_entry.path().extension()) {
                    VWolf::Ref<VWolf::Material> material = VWolf::MaterialSerializer::Deserialize(dir_entry.path());
                    materials[dir_entry.path()] = material;
                }
            }
        }
    }

    Project::~Project() {
        
    }

    void Project::AddObserver(std::uintptr_t key, std::function<void(const std::string& path, const filewatch::Event event)> value) {
        _observers[key] = value;
    }

    void Project::Save() {
        settings.Save();
        for (auto sceneKV: scenes) {
            VWolf::SceneSerializer::Serialize(sceneKV.second, sceneKV.first);
        }
        for (auto materialKV: materials) {
            VWolf::MaterialSerializer::Serialize(*materialKV.second, materialKV.first);
        }
    }

    std::filesystem::path Project::GetAssetsPath() {
        return GetProjectPath() / Folder::GetAssetsFolder();
    }

    void Project::InitializeCurrentProject(VWolf::Ref<Project> project) {
        Project::currentProject = project;
    }

    VWolf::Ref<Project> Project::CurrentProject() {
        return Project::currentProject;
    }

    VWolf::Ref<VWolf::Material> Project::GetMaterial(std::filesystem::path inPath) {
        if (materials.count(inPath))
            return materials[inPath];
        return nullptr;
    }

    VWolf::Ref<Project> Project::currentProject;
}
