//
//  Project.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#include "Project.h"

#include <yaml-cpp/yaml.h>

#include "Serialization/Project.h"

namespace VWolfPup {

    static std::string PROJECT_EXTENSION = ".vwolfproj";
    static std::string ASSETS_NAME = "Assets";

    void SetupProject(std::filesystem::path& path) {
        std::filesystem::create_directory(path);

        std::filesystem::path assetsPath = path;
        assetsPath /= ASSETS_NAME;
        std::filesystem::create_directory(assetsPath);
    
        std::array<std::string, 2> subfolders = { "Scenes", "Materials" };
        for (std::string subFolder: subfolders) {
            std::filesystem::create_directory(assetsPath / subFolder);
        }
    }

    bool ProjectExists(std::filesystem::path& path) {
        std::string fileName = path.filename().string() + PROJECT_EXTENSION;
        return std::filesystem::exists(path) && std::filesystem::exists(path / ASSETS_NAME) && std::filesystem::exists(path / fileName);
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

    void Project::Settings::Save() {
        YAML::Emitter out;
        out << *this;
        std::ofstream fout(path.string());
        fout << out.c_str();
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
        this->type = settings.type;
    }

    Project::Project(std::filesystem::path path):
    projectPath(path),
    settings((path / path.filename()).concat(PROJECT_EXTENSION)) {
        watch = new filewatch::FileWatch<std::string> {
            path.string(),
            [this] (const std::string& path, const filewatch::Event event) {
                for (const auto& kv : _observers) {
                    kv.second(path, event);
                }
            }
        };
    }

    Project::~Project() {
        
    }

    void Project::AddObserver(std::uintptr_t key, std::function<void(const std::string& path, const filewatch::Event event)> value) {
        _observers[key] = value;
    }

    void Project::Save() {
        settings.Save();
    }

    std::filesystem::path Project::GetAssetsPath() {
        return GetProjectPath() / ASSETS_NAME;
    }

    void Project::InitializeCurrentProject(VWolf::Ref<Project> project) {
        Project::currentProject = project;
    }

    VWolf::Ref<Project> Project::CurrentProject() {
        return Project::currentProject;
    }

    VWolf::Ref<Project> Project::currentProject;
}
