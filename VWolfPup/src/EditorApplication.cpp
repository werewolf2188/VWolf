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

#define NUMSHADERS 2
std::array<std::string, NUMSHADERS> shaderNames = { { "Flat Color", "Blinn Phon" } };
std::array<VWolf::ShaderSource, NUMSHADERS> vsFiles;
std::array<VWolf::ShaderSource, NUMSHADERS> psFiles;

void LoadShaderNames(VWolf::DriverType driverType) {
#ifdef VWOLF_PLATFORM_WINDOWS
    if (driverType == VWolf::DriverType::DirectX12) {

        vsFiles = { {
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/hlsl/FlatColor.hlsl" , "VS" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/hlsl/BlinnPhong.hlsl" , "VS" }
        } };

        psFiles = { {
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/hlsl/FlatColor.hlsl" , "PS" },
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/hlsl/BlinnPhong.hlsl" , "PS" }
        } };
    }
    else if (driverType == VWolf::DriverType::OpenGL) {

        vsFiles = { {
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/glsl/FlatColor.vert.glsl" , "main" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/glsl/BlinnPhong.vert.glsl" , "main" }
        } };

        psFiles = { {
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/glsl/FlatColor.frag.glsl" , "main" },
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/glsl/BlinnPhong.frag.glsl" , "main" }
        } };
    }
#else

    vsFiles = { {
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/FlatColor.vert.glsl" , "main" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/BlinnPhong.vert.glsl" , "main" }
        } };

    psFiles = { {
        { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/FlatColor.frag.glsl" , "main" },
        { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/BlinnPhong.frag.glsl" , "main" }
    } };
#endif
}

class RendererSandboxApplication: public VWolf::Application {
public:
    
    VWolf::Ref<VWolf::Camera> camera;
    VWolf::Material material_1;
    VWolf::Material material_2;
    VWolf::Ref<VWolf::Texture2D> testTexture;

    VWolf::Ref<VWolfPup::CameraController> controller;

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

    // Scene Management
    VWolf::Ref<VWolf::Scene> testScene;
//    VWolf::Ref<VWolf::GameObject> cylinder, sphere, grid, geosphere, box, light1, light2;
public:
    RendererSandboxApplication(): Application(DRIVER_TYPE, { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Renderer Sandbox" } ) {
        VWolfPup::InitializeEditor();
        
        camera = VWolf::CreateRef<VWolf::Camera>(30.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        controller = VWolf::CreateRef<VWolfPup::CameraController>(camera);

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
            VWOLF_CLIENT_INFO("Saving file %s", path.string().c_str());
            VWolf::SceneSerializer::Serialize(testScene, path);
        });
        containerView->AddView(saveBrowser);
        openBrowser = new VWolfPup::FileBrowser(VWolfPup::FileBrowserMode::Open, [this](std::filesystem::path path){
            VWOLF_CLIENT_INFO("Opening file %s", path.string().c_str());
            testScene = VWolf::SceneSerializer::Deserialize(path);
            sceneHierarchy->SetScene(testScene.get());
            sceneSettings->SetScene(testScene.get());
        });
        containerView->AddView(openBrowser);
        //
        LoadShaderNames(DRIVER_TYPE);
        
        for (int i = 0; i < NUMSHADERS; i++) {
            VWolf::ShaderLibrary::LoadShader(shaderNames[i].c_str(), { vsFiles[i], psFiles[i] });
        }

        new (&material_1) VWolf::Material(shaderNames[0].c_str());
        new (&material_2) VWolf::Material(shaderNames[1].c_str());

        material_1.SetColor("u_ambientColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_1.SetColor("u_diffuseColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_1.SetVector3("u_specular", { 0.8f, 0.8f, 0.8f });
        material_1.SetFloat("u_shinines", 20);
        material_2.SetColor("u_ambientColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_2.SetColor("u_diffuseColor", { 1.0f, 1.0f, 1.0f, 1.0f });
        material_2.SetVector3("u_specular", { 0.8f, 0.8f, 0.8f });
        material_2.SetFloat("u_shinines", 20);
        if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
#ifdef VWOLF_PLATFORM_WINDOWS
            testTexture = VWolf::Texture::LoadTexture2D("src/assets/textExample.png");
#else
            testTexture = VWolf::Texture::LoadTexture2D("../../../Sandbox/src/assets/textExample.png");
#endif            
            material_2.SetTexture("u_texture", testTexture);
        }
#ifdef VWOLF_PLATFORM_WINDOWS
        else {
            //testTexture = VWolf::Texture::LoadTexture2D(512, 512);
            testTexture = VWolf::Texture::LoadTexture2D("src/assets/textExample2.png");
            material_2.SetTexture("gDiffuseMap", testTexture);
        }
#endif

//        cylinder->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateCylinder(1, 1, 3, 32, 8), material_2);
//        sphere->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateSphere(2, 32, 32), material_2);
//        grid->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateGrid(2, 2, 16, 16), material_2);
//        geosphere->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateGeosphere(1, 4), material_2);
//        box->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateBox(1, 1, 1, 0), material_2);
//
//        light1->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateSphere(1, 32, 32), material_1);
//        light2->AddComponent<VWolf::ShapeRendererComponent>(VWolf::ShapeHelper::CreateBox(1, 1, 1, 0), material_1);
    }

    ~RendererSandboxApplication() {
    }

    void OnEvent(VWolf::Event& evt) override {
        VWolf::Application::OnEvent(evt);
        controller->OnEvent(evt);
        VWolf::Dispatch<VWolf::WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(RendererSandboxApplication::OnWindowResize));
    }

    void OnUpdate() override {
        testScene->UpdateEditor();
        controller->OnUpdate();
    }

    void OnDraw() override {
        if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
            VWolf::Graphics::Clear();
           
        }
        VWolf::Graphics::SetRenderTexture(sceneViewer->GetRenderTexture());
        testScene->DrawEditor(camera);        
        VWolf::Graphics::SetRenderTexture(nullptr);
    }

    void OnGUI() override {
        containerView->OnGui();
        ImGui::Begin("Texture");
        ImGui::Image(testTexture->GetHandler(), ImVec2(128, 128));
        ImGui::End();
    }

    bool OnWindowResize(VWolf::WindowResizeEvent& e) {
        if (e.GetWidth() != 0 && e.GetHeight() != 0)
            controller->SetViewportSize(e.GetWidth(), e.GetHeight());
        return true;
    }
};

VWOLF_MAIN_APP(RendererSandboxApplication)
