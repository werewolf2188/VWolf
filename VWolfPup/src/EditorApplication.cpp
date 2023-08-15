#include <iostream>

#include <imgui/imgui.h>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

#include <array>
#define _USE_MATH_DEFINES
#include <math.h>

#define SCREENWIDTH 1280.0f
#define SCREENHEIGHT 720.0f

#include "LoadSettings.h"
#include "Camera/EditorCamera.h"
#include "UI/UICore.h"
#include "UI/SceneHierarchy.h"
#include "UI/Inspector.h"
#include "UI/SceneViewer.h"
#include "UI/SceneSettings.h"
#include "UI/FileBrowser.h"
#include "UI/ProjectStructure.h"

#include "ProjectManagement/Project.h"

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
    VWolf::Material material_2;
    VWolf::Material materialGrid;
    // TODO: I still don't like to have the default material for the skybox living here.
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
    VWolfPup::ProjectStructure *projectStructure;

    // Scene Management
    VWolf::Ref<VWolf::Scene> testScene;
public:
    RendererSandboxApplication(): Application(VWolfPup::LoadProject(), { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Renderer Sandbox" } ) {
        VWolfPup::InitialLoad();
        VWolfPup::CreateGridMaterial(materialGrid);
        VWolfPup::CreateDefaultSkyboxMaterial(materialSkybox);
        VWolfPup::InitializeEditor();
        
        camera = VWolf::CreateRef<VWolf::Camera>(45.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        skyBoxCamera = VWolf::CreateRef<VWolf::Camera>(45.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        controller = VWolf::CreateRef<VWolfPup::CameraController>(camera);
        skyBoxController = VWolf::CreateRef<VWolfPup::CameraController>(skyBoxCamera);
        skyBoxController->SetUseDistanceAndFocalForPositionCalculation(false);

        // Scene
        testScene = VWolf::CreateRef<VWolf::Scene>("Test");

        // UI
        containerView = new VWolfPup::ContainerView("Test", {});
        quit = new VWolfPup::MenuItem("Quit", [this](std::string title) {
            // VWolfPup::Project::CurrentProject()->Save();
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

        sceneViewer = new VWolfPup::SceneViewer(camera, VWolfPup::Project::CurrentProject()->GetType(), (uint32_t)SCREENWIDTH, (uint32_t)SCREENHEIGHT);
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

        projectStructure = new VWolfPup::ProjectStructure();
        containerView->AddView(projectStructure);
        //

        VWolfPup::CreateDefaultMaterial(material_2);

        //material_2.SetVector3("u_specular", { 0.8f, 0.8f, 0.8f });
        //material_2.SetFloat("u_shinines", 20);
//        if (VWolfPup::Project::CurrentProject()->GetType() == VWolf::DriverType::OpenGL) {
//            testTexture = VWolf::Texture::LoadTexture2D("assets/textExample.png");
//            material_2.SetTexture("u_texture", testTexture);
//        }
//#ifdef VWOLF_PLATFORM_WINDOWS
//        if (VWolfPup::Project::CurrentProject()->GetType() == VWolf::DriverType::DirectX12) {
//            //testTexture = VWolf::Texture::LoadTexture2D(512, 512);
//            testTexture = VWolf::Texture::LoadTexture2D("assets/textExample2.png");
//            material_2.SetTexture("gDiffuseMap", testTexture);
//        }
//#endif

        testScene->GetSceneBackground().SetSkyboxMaterial(materialSkybox);
        testScene->GetSceneBackground().SetCamera(skyBoxCamera);

        VWolf::Graphics::SetRenderTexture(sceneViewer->GetRenderTexture());
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
        testScene->DrawEditor(camera);        
        VWolf::Graphics::RenderMesh(gridData, VWolf::MatrixFloat4x4(), materialGrid);
    }

    void OnGUI() override {
        containerView->OnGui();
//        ImGui::ShowDemoWindow();
    }

    bool OnWindowResize(VWolf::WindowResizeEvent& e) {
        if (e.GetWidth() != 0 && e.GetHeight() != 0)
            controller->SetViewportSize(e.GetWidth(), e.GetHeight());
        return true;
    }
};

VWOLF_MAIN_APP(RendererSandboxApplication)
