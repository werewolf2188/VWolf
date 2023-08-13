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

namespace VWolfPup {
    std::array<VWolf::ShaderSource, NUMSHADERS> vsFiles;
    std::array<VWolf::ShaderSource, NUMSHADERS> psFiles;

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

    void LoadShaderNames(VWolf::DriverType driverType) {
        if (driverType == VWolf::DriverType::OpenGL) {

            VWolfPup::vsFiles = { {
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/FlatColor.vert.glsl" , "main" },
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/BlinnPhong.vert.glsl" , "main" },
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/Grid.vert.glsl" , "main" },
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/Skybox.vert.glsl" , "main" }
            } };

            VWolfPup::psFiles = { {
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/glsl/FlatColor.frag.glsl" , "main" },
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/glsl/BlinnPhong.frag.glsl" , "main" },
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/glsl/Grid.frag.glsl" , "main" },
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/glsl/Skybox.frag.glsl" , "main" }
            } };
        }
    #ifdef VWOLF_PLATFORM_WINDOWS
        else  if (driverType == VWolf::DriverType::DirectX12) {

            vsFiles = { {
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/hlsl/FlatColor.hlsl" , "VS" },
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/hlsl/BlinnPhong.hlsl" , "VS" },
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/hlsl/Grid.hlsl" , "VS" },
                 { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/hlsl/Skybox.hlsl" , "VS" }
            } };

            psFiles = { {
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/hlsl/FlatColor.hlsl" , "PS" },
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/hlsl/BlinnPhong.hlsl" , "PS" },
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/hlsl/Grid.hlsl" , "PS" },
                { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/hlsl/Skybox.hlsl" , "PS" }
            } };
        }
    #endif

    }
// ----------------------------------------------- //

    // MARK: Private
    VWolf::ShaderConfiguration GetInitialConfiguration(std::string name) {
        if (name == DEFAULT_SKYBOX)
            return { VWolf::ShaderConfiguration::Rasterization(), { true, VWolf::ShaderConfiguration::DepthStencil::DepthFunction::LEqual }, VWolf::ShaderConfiguration::Blend() };
        return VWolf::ShaderConfiguration();
    }

    

    // MARK: Public
    void InitialLoad() {
        if (VWolfPup::Project::CurrentProject()->GetType() == VWolf::DriverType::OpenGL) {
            LoadGLSLShaders();
            return;
        }
        
        LoadShaderNames(VWolfPup::Project::CurrentProject()->GetType());
        for (int i = 0; i < NUMSHADERS; i++) {
            VWolf::ShaderLibrary::LoadShader(shaderNames[i], { vsFiles[i], psFiles[i] }, GetInitialConfiguration(shaderNames[i]));
        }

    }

    void CreateGridMaterial(VWolf::Material& material) {
        new (&material) VWolf::Material(DEFAULT_GRID.c_str());
    }

    void CreateDefaultSkyboxMaterial(VWolf::Material& material) {
        new (&material) VWolf::Material(DEFAULT_SKYBOX.c_str());
        //        materialSkybox.SetTexture("skybox", VWolf::Texture::LoadCubemap(512));
        material.SetTexture("skybox",
                            VWolf::Texture::LoadCubemap({ "assets/skybox/right.png",
                                                          "assets/skybox/left.png",
                                                          "assets/skybox/top.png",
                                                          "assets/skybox/bottom.png",
                                                          "assets/skybox/front.png",
                                                          "assets/skybox/back.png" }));
    }
}
