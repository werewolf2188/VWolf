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
#include "UI/Toolbar.h"

#include "ProjectManagement/Project.h"

#include "AssetManagement/AssetDatabase.h"

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

VWolf::Mesh CreateGridEx() {
    VWolf::Mesh meshData;
    meshData.SetName("Grid");
    meshData.GetVertices().resize(6);
    meshData.GetColors().resize(6);
    meshData.GetNormals().resize(6);
    meshData.GetTangents().resize(6);
    meshData.GetUVs().resize(6);
    meshData.GetTriangles().resize(6);

    meshData.GetTriangles()[0] = 0;
    meshData.GetTriangles()[1] = 1;
    meshData.GetTriangles()[2] = 2;
    meshData.GetTriangles()[3] = 3;
    meshData.GetTriangles()[4] = 4;
    meshData.GetTriangles()[5] = 5;
    return meshData;
}

class RendererSandboxApplication: public VWolf::Application {
public:
    bool isPlaying = false;
    VWolf::Ref<VWolf::Camera> camera, skyBoxCamera;
    VWolf::Ref<VWolf::Texture2D> testTexture;
    VWolf::MeshData gridData = CreateGrid();
    VWolf::Ref<VWolf::Mesh> gridDataEx = VWolf::CreateRef<VWolf::Mesh>(CreateGridEx());

    VWolf::Ref<VWolfPup::CameraController> controller, skyBoxController;

    // UI
    VWolfPup::ContainerView* containerView;
    VWolfPup::MenuItem *quit, *save, *createMetaFiles;
    VWolfPup::Menu *file, *assets;
    VWolfPup::MenuBar * menuBar;
    VWolfPup::SceneHierarchy *sceneHierarchy;
    VWolfPup::Inspector *inspector;
    VWolfPup::SceneViewer *sceneViewer;
    VWolfPup::SceneSettings* sceneSettings;
    VWolfPup::FileBrowser *saveBrowser, *openBrowser;
    VWolfPup::ProjectStructure *projectStructure;
    VWolfPup::Toolbar *toolbar;

    // Scene Management
    VWolf::Ref<VWolf::Scene> testScene;
public:
    RendererSandboxApplication(): Application(VWolfPup::LoadProject(), { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Renderer Sandbox" } ) {
        VWolfPup::Defaults::Load();
        VWolfPup::AssetDatabase::LoadMetaFilesForEditor();
        VWolfPup::InitializeEditor();
        
        camera = VWolf::CreateRef<VWolf::Camera>(45.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        VWolf::Camera::SetMainCamera(camera.get());
        skyBoxCamera = VWolf::CreateRef<VWolf::Camera>(45.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        controller = VWolf::CreateRef<VWolfPup::CameraController>(camera);
        skyBoxController = VWolf::CreateRef<VWolfPup::CameraController>(skyBoxCamera);
        skyBoxController->SetUseDistanceAndFocalForPositionCalculation(false);

        VWolfPup::Project::CurrentProject()->GetSettings().GetEditorCameraSettings().SetCameraControllerInformation(controller);
        VWolfPup::Project::CurrentProject()->GetSettings().GetEditorCameraSettings().SetCameraControllerInformation(skyBoxController);

        // Scene
        testScene =  VWolfPup::Project::CurrentProject()->GetCurrentScene();
//        testScene =  VWolf::CreateRef<VWolf::Scene>("Test");

        // UI
        containerView = new VWolfPup::ContainerView("Test", {});
        quit = new VWolfPup::MenuItem("Quit", [this](std::string title) {
            VWolfPup::Project::CurrentProject()->GetSettings().GetEditorCameraSettings().GetCameraControllerInformation(controller);
            VWolfPup::Project::CurrentProject()->Save();
            this->containerView->SaveIniFile();
            this->Quit();
        });
//        open = new VWolfPup::MenuItem("Open Scene", [this](std::string title) {
//            openBrowser->Open();
//        });
        save = new VWolfPup::MenuItem("Save", [this](std::string title) {
            VWolfPup::Project::CurrentProject()->GetSettings().GetEditorCameraSettings().GetCameraControllerInformation(controller);
            VWolfPup::Project::CurrentProject()->Save();
        });
        file = new VWolfPup::Menu("File", { save, new VWolfPup::MenuItem(), quit });
        
        createMetaFiles = new VWolfPup::MenuItem("Recreate meta files", [this](std::string title) {
            VWolfPup::AssetDatabase::CreateMetaFilesForEditor();
        });
        
        assets = new VWolfPup::Menu("Assets", { createMetaFiles });
        
        menuBar = new VWolfPup::MenuBar("MenuBar", { file, assets });
        containerView->SetMenuBar(menuBar);

        inspector = new VWolfPup::Inspector();
        containerView->AddView(inspector);

        sceneViewer = new VWolfPup::SceneViewer(camera, VWolfPup::Project::CurrentProject()->GetType(), (uint32_t)SCREENWIDTH, (uint32_t)SCREENHEIGHT);
        containerView->AddView(sceneViewer);

        sceneSettings = new VWolfPup::SceneSettings(VWolfPup::Project::CurrentProject()->GetCurrentScene().get());
        containerView->AddView(sceneSettings);

        sceneHierarchy = new VWolfPup::SceneHierarchy(VWolfPup::Project::CurrentProject()->GetCurrentScene().get(), [this](VWolf::Ref<VWolf::GameObject> gameObject) {
            inspector->SetGameObject(gameObject);
            sceneViewer->SetSelectedObject(gameObject);
        });
        containerView->AddView(sceneHierarchy);

        saveBrowser = new VWolfPup::FileBrowser(VWolfPup::FileBrowserMode::Save, [this](std::filesystem::path path){
//            VWOLF_CLIENT_INFO("Saving file %s", path.string().c_str());
//            VWolf::SceneSerializer::Serialize(VWolfPup::Project::CurrentProject()->GetCurrentScene(), path);
        });
        containerView->AddView(saveBrowser);
        // TODO: This should not come from opening a scene anymore, but from double clicking on a scene file inside assets.
        openBrowser = new VWolfPup::FileBrowser(VWolfPup::FileBrowserMode::Open, [this](std::filesystem::path path){
////            VWOLF_CLIENT_INFO("Opening file %s", path.string().c_str());
//            sceneViewer->SetSelectedObject(nullptr);
//            inspector->SetGameObject(nullptr);
//            testScene = VWolf::SceneSerializer::Deserialize(path);
//            testScene->GetSceneBackground().SetSkyboxMaterial(*VWolfPup::Defaults::Get()->GetDefaultSkyBoxMaterial());
//            testScene->GetSceneBackground().SetCamera(skyBoxCamera);
//            sceneHierarchy->SetScene(testScene.get());
//            sceneSettings->SetScene(testScene.get());
        });
        containerView->AddView(openBrowser);

        projectStructure = new VWolfPup::ProjectStructure();
        containerView->AddView(projectStructure);

        toolbar = new VWolfPup::Toolbar([&](bool isPlaying){
            this->isPlaying = isPlaying;
            inspector->SetGameObject(nullptr);
            sceneViewer->SetSelectedObject(nullptr);
            sceneViewer->SetPlaying(isPlaying);
            if (isPlaying)
                testScene->StartingPreview();
            else {
                controller->OnUpdate();
                skyBoxController->OnUpdate();
                testScene->StopingPreview();
            }                
        });
        containerView->AddView(toolbar);
        //

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
        std::string skyMaterialName = VWolfPup::Defaults::Get()->GetDefaultSkyBoxMaterialName();
        VWolf::Ref<VWolf::Material> skyMaterial = VWolf::MaterialLibrary::GetMaterial(skyMaterialName);
        testScene->GetSceneBackground().SetSkyboxMaterial(*skyMaterial);
        // TODO: This should come from the same camera.
        testScene->GetSceneBackground().SetCamera(skyBoxCamera);

        VWolf::Graphics::SetRenderTexture(sceneViewer->GetRenderTexture());
    }

    ~RendererSandboxApplication() {}

    void OnEvent(VWolf::Event& evt) override {
        VWolf::Application::OnEvent(evt);
        VWolf::Dispatch<VWolf::WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(RendererSandboxApplication::OnWindowResize));
        VWolf::Dispatch<VWolf::WindowCloseEvent>(evt, VWOLF_BIND_EVENT_FN(RendererSandboxApplication::OnWindowClose));

        sceneHierarchy->OnEvent(evt);
        projectStructure->OnEvent(evt);

        if (openBrowser->IsOpen() || saveBrowser->IsOpen() || isPlaying) return;
        if (sceneViewer->IsHovering()) {
            controller->OnEvent(evt);
            skyBoxController->OnEvent(evt);
        }
    }

    bool OnWindowClose(VWolf::WindowCloseEvent& e) {
        this->containerView->SaveIniFile();
        return true;
    }

    void OnUpdate() override {
        testScene->UpdateEditor();

        if (openBrowser->IsOpen() || saveBrowser->IsOpen() || isPlaying) return;

        if (sceneViewer->IsHovering()) {
            controller->OnUpdate();
            skyBoxController->OnUpdate();
        }
    }

    void OnDraw() override {
        if (isPlaying)
            testScene->DrawPreviewEditor();
        else {
            testScene->DrawEditor(camera);
            VWolf::Graphics::RenderMesh(gridDataEx, gridData, VWolf::Matrix4x4(), *VWolf::MaterialLibrary::GetMaterial(VWolfPup::Defaults::Get()->GetDefaultGridMaterialName()));
        }
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
