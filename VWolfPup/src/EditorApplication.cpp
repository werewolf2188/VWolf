#include <iostream>

#include <imgui/imgui.h>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

#include <array>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef VWOLF_PLATFORM_WINDOWS
#define DRIVER_TYPE VWolf::DriverType::DirectX12 
#else
#define DRIVER_TYPE VWolf::DriverType::OpenGL
#endif

#define SCREENWIDTH 1280.0f
#define SCREENHEIGHT 720.0f

#include "Camera/EditorCamera.h"
#include "UI/UICore.h"
#include "UI/SceneHierarchy.h"
#include "UI/Inspector.h"
#include "UI/SceneViewer.h"
#include "UI/SceneSettings.h"
#include "UI/FileBrowser.h"

#define NUMSHADERS 4
std::array<std::string, NUMSHADERS> shaderNames = { { "Flat Color", "Blinn Phon", "Grid", "Skybox" } };
std::array<VWolf::ShaderSource, NUMSHADERS> vsFiles;
std::array<VWolf::ShaderSource, NUMSHADERS> psFiles;
std::array<VWolf::ShaderConfiguration, NUMSHADERS> configurations;

void LoadShaderNames(VWolf::DriverType driverType) {
    if (driverType == VWolf::DriverType::OpenGL) {

        vsFiles = { {
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/FlatColor.vert.glsl" , "main" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/BlinnPhong.vert.glsl" , "main" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/Grid.vert.glsl" , "main" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/glsl/Skybox.vert.glsl" , "main" }
        } };

        psFiles = { {
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
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "shaders/hlsl/Grid.hlsl" , "VS" }
        } };

        psFiles = { {
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/hlsl/FlatColor.hlsl" , "PS" },
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/hlsl/BlinnPhong.hlsl" , "PS" },
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "shaders/hlsl/Grid.hlsl" , "PS" }
        } };
    }
#endif
    configurations = { {
        VWolf::ShaderConfiguration(),
        VWolf::ShaderConfiguration(),
        VWolf::ShaderConfiguration(),
        { VWolf::ShaderConfiguration::Rasterization(), { true, VWolf::ShaderConfiguration::DepthStencil::DepthFunction::LEqual }, VWolf::ShaderConfiguration::Blend() }
    } };
}

class TextureView: public VWolfPup::View {
public:
    TextureView(VWolf::Ref<VWolf::Texture2D> testTexture):
    View("Texture"), testTexture(testTexture) {}
    ~TextureView() {}
public:
    virtual void OnGui() override {
        ImGui::Begin(title.c_str());
        ImGui::Image(testTexture->GetHandler(), ImVec2(128, 128));
        ImGui::End();
    }
protected:
    virtual void SetInContainer() override {
        GetContainer()->GetRoot()->Install(this, ImGuiDir_Down);
    }
private:
    VWolf::Ref<VWolf::Texture2D> testTexture;
};

VWolf::MeshData CreateGrid() {
    VWolf::MeshData meshData;
    meshData.SetName("Grid");
    meshData.vertices.resize(6);
    meshData.indices.resize(6);

    meshData.indices[0] = 0;
    meshData.indices[1] = 1;
    meshData.indices[2] = 2;
    meshData.indices[3] = 3;
    meshData.indices[4] = 4;
    meshData.indices[5] = 5;
    return meshData;
}

class RendererSandboxApplication: public VWolf::Application {
public:
    
    VWolf::Ref<VWolf::Camera> camera, skyBoxCamera;
    VWolf::Material material_1;
    VWolf::Material material_2;
    VWolf::Material materialGrid;
    // TODO: I still don't like to have the default materiaal for the skybox living here.
    VWolf::Material materialSkybox;
    VWolf::Ref<VWolf::Texture2D> testTexture;
    VWolf::MeshData gridData = CreateGrid();

    VWolf::Ref<VWolfPup::CameraController> controller, skyBoxController;

    // UI
    VWolfPup::ContainerView* containerView;
    VWolfPup::MenuItem *quit, *save, *open;
    VWolfPup::Menu * file;
    VWolfPup::MenuBar * menuBar;
    VWolfPup::SceneHierarchy *sceneHierarchy;
    VWolfPup::Inspector *inspector;
    VWolfPup::SceneViewer *sceneViewer;
    VWolfPup::SceneSettings* sceneSettings;
    VWolfPup::FileBrowser *saveBrowser, *openBrowser;
    TextureView* textureView;

    // Scene Management
    VWolf::Ref<VWolf::Scene> testScene;
//    VWolf::Ref<VWolf::GameObject> cylinder, sphere, grid, geosphere, box, light1, light2;
public:
    RendererSandboxApplication(): Application(DRIVER_TYPE, { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Renderer Sandbox" } ) {
        VWolfPup::InitializeEditor();
        
        camera = VWolf::CreateRef<VWolf::Camera>(30.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        skyBoxCamera = VWolf::CreateRef<VWolf::Camera>(30.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        controller = VWolf::CreateRef<VWolfPup::CameraController>(camera);
        skyBoxController = VWolf::CreateRef<VWolfPup::CameraController>(skyBoxCamera);
        skyBoxController->SetUseDistanceAndFocalForPositionCalculation(false);

        // Scene
        testScene = VWolf::CreateRef<VWolf::Scene>("Test");
//        cylinder = testScene->CreateGameObject("Cylinder");
//        sphere = testScene->CreateGameObject("Sphere");
//        grid = testScene->CreateGameObject("Grid");
//        geosphere = testScene->CreateGameObject("Geosphere");
//        box = testScene->CreateGameObject("Box");
//
//        light1 = testScene->CreateGameObject("Light 1");
//        light2 = testScene->CreateGameObject("Light 2");
//
//        light1->AddComponent<VWolf::LightComponent>();
//        light2->AddComponent<VWolf::LightComponent>();
        //

        // UI
        containerView = new VWolfPup::ContainerView("Test", {});
        quit = new VWolfPup::MenuItem("Quit", [this](std::string title) {
            this->containerView->SaveIniFile();
            this->Quit();
        });
        open = new VWolfPup::MenuItem("Open Scene", [this](std::string title) {
            openBrowser->Open();
        });
        save = new VWolfPup::MenuItem("Save Scene", [this](std::string title) {
            saveBrowser->Open();
        });
        file = new VWolfPup::Menu("File", { open, save, new VWolfPup::MenuItem(), quit });
        menuBar = new VWolfPup::MenuBar("MenuBar", { file });
        containerView->SetMenuBar(menuBar);

        inspector = new VWolfPup::Inspector();
        containerView->AddView(inspector);

        sceneViewer = new VWolfPup::SceneViewer(camera, DRIVER_TYPE, (uint32_t)SCREENWIDTH, (uint32_t)SCREENHEIGHT);
        containerView->AddView(sceneViewer);

        sceneSettings = new VWolfPup::SceneSettings(testScene.get());
        containerView->AddView(sceneSettings);

        sceneHierarchy = new VWolfPup::SceneHierarchy(testScene.get(), [this](VWolf::Ref<VWolf::GameObject> gameObject) {
            inspector->SetGameObject(gameObject);
            sceneViewer->SetSelectedObject(gameObject);
        });
        containerView->AddView(sceneHierarchy);

        saveBrowser = new VWolfPup::FileBrowser(VWolfPup::FileBrowserMode::Save, [this](std::filesystem::path path){
//            VWOLF_CLIENT_INFO("Saving file %s", path.string().c_str());
            VWolf::SceneSerializer::Serialize(testScene, path);
        });
        containerView->AddView(saveBrowser);
        openBrowser = new VWolfPup::FileBrowser(VWolfPup::FileBrowserMode::Open, [this](std::filesystem::path path){
//            VWOLF_CLIENT_INFO("Opening file %s", path.string().c_str());
            sceneViewer->SetSelectedObject(nullptr);
            inspector->SetGameObject(nullptr);
            testScene = VWolf::SceneSerializer::Deserialize(path);
            testScene->GetSceneBackground().SetSkyboxMaterial(materialSkybox);
            testScene->GetSceneBackground().SetCamera(skyBoxCamera);
            sceneHierarchy->SetScene(testScene.get());
            sceneSettings->SetScene(testScene.get());
        });
        containerView->AddView(openBrowser);
        //
        LoadShaderNames(DRIVER_TYPE);
        
        for (int i = 0; i < NUMSHADERS; i++) {
            VWolf::ShaderLibrary::LoadShader(shaderNames[i].c_str(), { vsFiles[i], psFiles[i] }, configurations[i]);
        }

        new (&material_1) VWolf::Material(shaderNames[0].c_str());
        new (&material_2) VWolf::Material(shaderNames[1].c_str());
        new (&materialGrid) VWolf::Material(shaderNames[2].c_str());
        new (&materialSkybox) VWolf::Material(shaderNames[3].c_str());

        material_2.SetAsDefault();
        material_1.SetColor("u_ambientColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_1.SetColor("u_diffuseColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_1.SetVector3("u_specular", { 0.8f, 0.8f, 0.8f });
        material_1.SetFloat("u_shinines", 20);
        material_2.SetColor("u_ambientColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_2.SetColor("u_diffuseColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_2.SetVector3("u_specular", { 0.8f, 0.8f, 0.8f });
        material_2.SetFloat("u_shinines", 20);
        if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
            testTexture = VWolf::Texture::LoadTexture2D("assets/textExample.png");
            material_2.SetTexture("u_texture", testTexture);
        }
#ifdef VWOLF_PLATFORM_WINDOWS
        else if (DRIVER_TYPE == VWolf::DriverType::DirectX12) {
            //testTexture = VWolf::Texture::LoadTexture2D(512, 512);
            testTexture = VWolf::Texture::LoadTexture2D("assets/textExample2.png");
            material_2.SetTexture("gDiffuseMap", testTexture);
        }
#endif
//        materialSkybox.SetTexture("skybox", VWolf::Texture::LoadCubemap(512));
        materialSkybox.SetTexture("skybox",
                                  VWolf::Texture::LoadCubemap({ "assets/skybox/right.png",
                                                                "assets/skybox/left.png",
                                                                "assets/skybox/top.png",
                                                                "assets/skybox/bottom.png",
                                                                "assets/skybox/front.png",
                                                                "assets/skybox/back.png" }));
        testScene->GetSceneBackground().SetSkyboxMaterial(materialSkybox);
        testScene->GetSceneBackground().SetCamera(skyBoxCamera);
        textureView = new TextureView(testTexture);
        containerView->AddView(textureView);
//        cylinder->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateCylinder(1, 1, 3, 32, 8), material_2);
//        sphere->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateSphere(2, 32, 32), material_2);
//        grid->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateGrid(2, 2, 16, 16), material_2);
//        geosphere->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateGeosphere(1, 4), material_2);
//        box->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateBox(1, 1, 1, 0), material_2);
//
//        light1->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateSphere(1, 32, 32), material_1);
//        light2->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateBox(1, 1, 1, 0), material_1);
    }

    ~RendererSandboxApplication() {}

    void OnEvent(VWolf::Event& evt) override {
        VWolf::Application::OnEvent(evt);
        VWolf::Dispatch<VWolf::WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(RendererSandboxApplication::OnWindowResize));
        VWolf::Dispatch<VWolf::WindowCloseEvent>(evt, VWOLF_BIND_EVENT_FN(RendererSandboxApplication::OnWindowClose));

        if (openBrowser->IsOpen() || saveBrowser->IsOpen()) return;
        if (sceneViewer->IsHovering()) {
            controller->OnEvent(evt);
            skyBoxController->OnEvent(evt);
        }
        
        sceneHierarchy->OnEvent(evt);
    }

    bool OnWindowClose(VWolf::WindowCloseEvent& e) {
        this->containerView->SaveIniFile();
        return true;
    }

    void OnUpdate() override {
        if (openBrowser->IsOpen() || saveBrowser->IsOpen()) return;
        testScene->UpdateEditor();
        if (sceneViewer->IsHovering()) {
            controller->OnUpdate();
            skyBoxController->OnUpdate();
        }
    }

    void OnDraw() override {
        
        VWolf::Graphics::SetRenderTexture(sceneViewer->GetRenderTexture());

        testScene->DrawEditor(camera);
        
        VWolf::Graphics::RenderMesh(gridData, VWolf::MatrixFloat4x4(), materialGrid);
        VWolf::Graphics::SetRenderTexture(nullptr);
    }

    void OnGUI() override {
        containerView->OnGui();
    }

    bool OnWindowResize(VWolf::WindowResizeEvent& e) {
        if (e.GetWidth() != 0 && e.GetHeight() != 0)
            controller->SetViewportSize(e.GetWidth(), e.GetHeight());
        return true;
    }
};

VWOLF_MAIN_APP(RendererSandboxApplication)
