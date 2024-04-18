//
//  LoadSettings.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 8/13/23.
//

#include "LoadSettings.h"

#include <filesystem>
#include <vector>

#include "../ProjectManagement/Project.h"
#include "../ProjectManagement/Extensions.h"
#include "../ProjectManagement/Folder.h"

#include <yaml-cpp/yaml.h>
#include "Serialization/DefaultSettings.h"

namespace VWolfPup {

    // MARK: Defaults
    VWolf::Ref<Defaults> Defaults::defaults;

    Defaults::Defaults(Defaults& defaults) {
        this->defaultMaterial = defaults.defaultMaterial;
        this->defaultGridMaterial = defaults.defaultGridMaterial;
        this->defaultSkyBoxMaterial = defaults.defaultSkyBoxMaterial;
    }

    bool Defaults::IsDefault(VWolf::Material& material) {
        for(auto defaultMaterial: materials) {
            if (defaultMaterial.second->GetName() == material.GetName()) {
                return true;
            }
        }
        return false;
    }

    void Defaults::Load() {
        constexpr const char * fileName = "defaults.ini";
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(fileName);
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CLIENT_ERROR("Failed to load .scene file '%s'\n     %s", fileName, e.what());
        }

        Defaults defaults = data.as<Defaults>();
        Defaults::defaults = VWolf::CreateRef<Defaults>(defaults);
        Defaults::defaults->PrepareMaterials();
    }

    void Defaults::PrepareMaterials() {
        std::filesystem::path materialsPath = Folder::GetAssetsFolder() + "/Materials";
        std::string materialExtension = Extension::GetMaterialExtension();

        std::filesystem::path defaultMaterialPath = std::filesystem::current_path() / materialsPath / (defaultMaterial + materialExtension);
        std::filesystem::path defaultMaterialGridPath = std::filesystem::current_path() / materialsPath / (defaultGridMaterial + materialExtension);
        std::filesystem::path defaultMaterialSkyboxPath = std::filesystem::current_path() / materialsPath / (defaultSkyBoxMaterial + materialExtension);

        auto dfMat = VWolf::MaterialSerializer::Deserialize(defaultMaterialPath);
        dfMat->SetAsDefault();
        materials[defaultMaterial] = dfMat;
        auto dfgMat = VWolf::MaterialSerializer::Deserialize(defaultMaterialGridPath);
        materials[defaultGridMaterial] = dfgMat;
        auto dfskbMat = VWolf::MaterialSerializer::Deserialize(defaultMaterialSkyboxPath);
        dfskbMat->SetTexture("skybox",
                            VWolf::Texture::LoadCubemap({ "assets/skybox/right.png",
                                                          "assets/skybox/left.png",
                                                          "assets/skybox/top.png",
                                                          "assets/skybox/bottom.png",
                                                          "assets/skybox/front.png",
                                                          "assets/skybox/back.png" }));
        materials[defaultSkyBoxMaterial] = dfskbMat;
        
    }

    std::vector<VWolf::Ref<VWolf::Material>> materials;
    VWolf::ShaderConfiguration GetInitialConfiguration(std::string name);

    void LoadGLSLShaders() {
        std::filesystem::path shaderPath = "shaders/glsl/";
        std::string vertExtension = ".vert";
        std::string fragExtension = ".frag";

        std::vector<std::string> vertShaders, fragShaders, names;
        
        for(auto const& dir_entry : std::filesystem::directory_iterator(shaderPath)) {
            std::string filename = dir_entry.path().filename().string();
            
            if (filename.find(vertExtension) != std::string::npos) {
                vertShaders.push_back(filename);
                std::string name = filename.substr(0, filename.find(vertExtension));
                names.push_back(name);
    //                VWOLF_CLIENT_INFO("%s", name.c_str());
            }
            else if (filename.find(fragExtension) != std::string::npos) {
                fragShaders.push_back(filename);
    //                VWOLF_CLIENT_INFO("%s", filename.c_str());
            }
        }

        for(int index = 0; index < names.size(); index++) {
            std::string name = names[index];
            auto vertItem = std::find(vertShaders.begin(), vertShaders.end(), name + vertExtension + ".glsl");
            
            auto fragItem = std::find(fragShaders.begin(), fragShaders.end(), name + fragExtension + ".glsl");
            
            std::string vsFile, fsFile;
            if (vertItem != vertShaders.end() && fragItem != fragShaders.end()) {
                vsFile = (shaderPath / *vertItem).string();
                fsFile = (shaderPath / *fragItem).string();

                VWolf::ShaderSource vs = {
                    VWolf::ShaderType::Vertex,
                    VWolf::ShaderSourceType::File,
                    vsFile,
                    "main"
                };

                VWolf::ShaderSource fs = {
                    VWolf::ShaderType::Fragment,
                    VWolf::ShaderSourceType::File,
                    fsFile,
                    "main"
                };
                VWolf::ShaderLibrary::LoadShader(name, { vs, fs }, GetInitialConfiguration(name));
//                VWOLF_CLIENT_INFO("Test");
            }
        }
    }

    void LoadHLSLShaders() {
        std::filesystem::path shaderPath = "shaders/hlsl/";
        for (auto const& dir_entry : std::filesystem::directory_iterator(shaderPath)) {
            std::string filename = dir_entry.path().string();
            std::string name = dir_entry.path().stem().string();
            VWolf::ShaderSource vs = {
                VWolf::ShaderType::Vertex,
                VWolf::ShaderSourceType::File,
                filename,
                "VS"
            };

            VWolf::ShaderSource fs = {
                VWolf::ShaderType::Fragment,
                VWolf::ShaderSourceType::File,
                filename,
                "PS"
            };
            VWolf::ShaderLibrary::LoadShader(name, { vs, fs }, GetInitialConfiguration(name));
        }
    }
// ----------------------------------------------- //

    // MARK: Private
    VWolf::ShaderConfiguration GetInitialConfiguration(std::string name) {
        static const std::string DEFAULT_SKYBOX = "Skybox";
        if (name == DEFAULT_SKYBOX)
            return { VWolf::ShaderConfiguration::Rasterization(), { true, VWolf::ShaderConfiguration::DepthStencil::DepthFunction::LEqual }, VWolf::ShaderConfiguration::Blend() };
        // TODO: Work on these settings
//        if (name == DEFAULT_GRID)
//            return { VWolf::ShaderConfiguration::Rasterization(), { true, VWolf::ShaderConfiguration::DepthStencil::DepthFunction::LEqual }, VWolf::ShaderConfiguration::Blend() };
        return VWolf::ShaderConfiguration();
    }

    // MARK: Public
    void InitialLoad() {
        switch (VWolfPup::Project::CurrentProject()->GetType()) {
        case VWolf::DriverType::OpenGL:
            LoadGLSLShaders();
            break;
#ifdef VWOLF_PLATFORM_WINDOWS
        case VWolf::DriverType::DirectX12:
            LoadHLSLShaders();
            break;
#endif
        }
        Defaults::Load();
    }
}
