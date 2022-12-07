#include <iostream>

#include <imgui/imgui.h>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

#include <array>

#ifdef VWOLF_PLATFORM_WINDOWS
# define M_PI           3.14159265358979323846  /* pi TODO: Move later*/
#define DRIVER_TYPE VWolf::DriverType::DirectX12
#else
#define DRIVER_TYPE VWolf::DriverType::OpenGL
#endif

#define SCREENWIDTH 1280.0f
#define SCREENHEIGHT 720.0f

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

struct Albedo {
public:
    VWolf::Color ambientColor;
    VWolf::Color diffuseColor;
    VWolf::Vector3Float specular;
    float shinines;
};

struct Transform {
    VWolf::Vector3Float position;
    VWolf::Vector3Float rotation;
    VWolf::Vector3Float scale;
    VWolf::MatrixFloat4x4 matrix;

    Transform() {
        matrix = VWolf::MatrixFloat4x4(1.0f);
        position = VWolf::Vector3Float(0.0f);
        rotation = VWolf::Vector3Float(0.0f);
        scale = VWolf::Vector3Float(1.0f);
    }
    
    void Apply() {
        matrix = VWolf::translate(VWolf::MatrixFloat4x4(1.0f), position);
        matrix = VWolf::rotate(matrix, VWolf::radians(rotation.x), { 1.0f, 0.0f, 0.0f });
        matrix = VWolf::rotate(matrix, VWolf::radians(rotation.y), { 0.0f, 1.0f, 0.0f });
        matrix = VWolf::rotate(matrix, VWolf::radians(rotation.z), { 0.0f, 0.0f, 1.0f });
        matrix = VWolf::scale(matrix, scale);
    }
};

class GameObject {
public:
    Transform transform;
public:
    GameObject(VWolf::MeshData data, const char* id) {
        m_data = data;
        m_id = id;
        transform = Transform();
    }

    const char* GetId() {
        return m_id;
    }

    VWolf::MeshData& GetData() {
        return m_data;
    }
private:
    VWolf::MeshData m_data;
    const char* m_id;
};

struct LightInfo {
    VWolf::Light light;
    VWolf::MatrixFloat4x4 lightMatrix;
    VWolf::Vector4Float lightRotation;
};

class RendererSandboxApplication: public VWolf::Application {
public:
    VWolf::Ref<VWolf::PerspectiveCamera> camera;
    std::vector<VWolf::Ref<GameObject>> gameObjects1, gameObjects2;
    VWolf::Material<Albedo> material1;
    VWolf::Material<Albedo> material2;
//    LightInfo lightInfo;
    std::vector<LightInfo> lights;
    Albedo* mat1;
    Albedo* mat2;

    VWolf::MeshData pointMesh, directionalMesh, spotMesh;
    
    const char *materialName = "Material";
public:
    RendererSandboxApplication(): Application(DRIVER_TYPE, { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Renderer Sandbox" } ) {
        camera = VWolf::CreateRef<VWolf::PerspectiveCamera>(30.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        LoadShaderNames(DRIVER_TYPE);
        
        material1 = VWolf::Material<Albedo>(shaderNames[0].c_str(), materialName);
        mat1 = material1.GetChildObject();
        mat1->ambientColor = { 1.0f, 0.0f, 0.0f, 1.0f };
        mat1->diffuseColor = { 0.2f, 0.3f, 0.5f, 1.0f };
        mat1->specular = { 0.8f, 0.8f, 0.8f };
        mat1->shinines = 20;
        material2 = VWolf::Material<Albedo>(shaderNames[1].c_str(), materialName);
        mat2 = material2.GetChildObject();
        mat2->ambientColor = { 1.0f, 0.3f, 0.2f, 1.0f };
        mat2->diffuseColor = { 0.2f, 0.3f, 0.5f, 1.0f };
        mat2->specular = { 0.8f, 0.8f, 0.8f };
        mat2->shinines = 20;
        
        float radius = 1.0f;
        float theta = M_PI * 1.25;
        float phi = M_PI / 4;

        LightInfo lightInfo1;
        lightInfo1.light.direction = { radius * sinf(phi) * cosf(theta), radius * cosf(phi), radius * sinf(phi) * sinf(theta), 0.0f };
        lightInfo1.light.direction = -lightInfo1.light.direction;
        lightInfo1.light.type = VWolf::Light::LightType::Directional;

        lightInfo1.lightRotation = VWolf::degrees(lightInfo1.light.direction);
        lightInfo1.light.color = { 1.0f, 1.0f, 0.0f, 1.0f };
        lightInfo1.light.strength = { 0.5f, 0.5f, 0.5f, 0.5f };
        lightInfo1.light.position = { 6.0f, 3.0f, 0.0f, 1.0f };

        lightInfo1.light.cutOff = VWolf::radians(15.0f);
        lightInfo1.light.exponent = 50.0f;
        
        lightInfo1.lightMatrix = VWolf::translate(VWolf::MatrixFloat4x4(1.0f), VWolf::Vector3Float(lightInfo1.light.position));
        lightInfo1.lightMatrix = VWolf::rotate(lightInfo1.lightMatrix, lightInfo1.light.direction.x, { 1.0f, 0.0f, 0.0f });
        lightInfo1.lightMatrix = VWolf::rotate(lightInfo1.lightMatrix, lightInfo1.light.direction.y, { 0.0f, 1.0f, 0.0f });
        lightInfo1.lightMatrix = VWolf::rotate(lightInfo1.lightMatrix, lightInfo1.light.direction.z, { 0.0f, 0.0f, 1.0f });
        lights.push_back(lightInfo1);
        LightInfo lightInfo2;
        lightInfo2.light.direction = { radius * sinf(phi) * cosf(theta), radius * cosf(phi), radius * sinf(phi) * sinf(theta), 0.0f };
        lightInfo2.light.direction = -lightInfo2.light.direction;
        lightInfo2.light.type = VWolf::Light::LightType::Point;

        lightInfo2.lightRotation = VWolf::degrees(lightInfo2.light.direction);
        lightInfo2.light.color = { 1.0f, 0.0f, 0.0f, 1.0f };
        lightInfo2.light.strength = { 0.5f, 0.5f, 0.5f, 0.5f };
        lightInfo2.light.position = { -6.0f, 8.0f, 0.0f, 1.0f };

        lightInfo2.light.cutOff = VWolf::radians(15.0f);
        lightInfo2.light.exponent = 50.0f;
        
        lightInfo2.lightMatrix = VWolf::translate(VWolf::MatrixFloat4x4(1.0f), VWolf::Vector3Float(lightInfo2.light.position));
        lightInfo2.lightMatrix = VWolf::rotate(lightInfo2.lightMatrix, lightInfo2.light.direction.x, { 1.0f, 0.0f, 0.0f });
        lightInfo2.lightMatrix = VWolf::rotate(lightInfo2.lightMatrix, lightInfo2.light.direction.y, { 0.0f, 1.0f, 0.0f });
        lightInfo2.lightMatrix = VWolf::rotate(lightInfo2.lightMatrix, lightInfo2.light.direction.z, { 0.0f, 0.0f, 1.0f });
        lights.push_back(lightInfo2);
        
        std::initializer_list<VWolf::ShaderParameter> parameters = {
            { materialName, VWolf::ShaderParameterType::In, 2, material1.GetSize() },
            { VWolf::Light::LightName, VWolf::ShaderParameterType::In, 3, (sizeof(VWolf::Light) * VWolf::Light::LightsMax) } // MAX Lights = 8
           };
        for (int i = 0; i < NUMSHADERS; i++) {
            VWolf::ShaderLibrary::LoadShader(shaderNames[i].c_str(), vsFiles[i], { psFiles[i] }, parameters);
        }

        gameObjects1.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateCylinder(1, 1, 3, 32, 8), "0" ));
        gameObjects1.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateSphere(2, 32, 32), "1" ));
        gameObjects1.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateGrid(2, 2, 16, 16), "2" ));
        gameObjects2.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateGeosphere(1, 4), "3" ));
        gameObjects2.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateBox(1, 1, 1, 0), "4" ));
        
        pointMesh = VWolf::ShapeHelper::CreateSphere(1, 32, 32);
        directionalMesh = VWolf::ShapeHelper::CreateBox(1, 1, 1, 0);
        spotMesh = VWolf::ShapeHelper::CreateCylinder(1, 1, 3, 32, 32);
    }

    ~RendererSandboxApplication() {
    }

    void OnEvent(VWolf::Event& evt) override {
        VWolf::Application::OnEvent(evt);
        camera->OnEvent(evt);
        VWolf::Dispatch<VWolf::WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(RendererSandboxApplication::OnWindowResize));
    }

    void OnUpdate() override {
        camera->OnUpdate();
        for(auto gameObject: gameObjects1)
            gameObject->transform.Apply();
        for(auto gameObject: gameObjects2)
            gameObject->transform.Apply();
        
        for (LightInfo& lightInfo: lights) {
            lightInfo.light.direction = VWolf::radians(lightInfo.lightRotation);
            lightInfo.lightMatrix = VWolf::translate(VWolf::MatrixFloat4x4(1.0f), VWolf::Vector3Float(lightInfo.light.position));
            lightInfo.lightMatrix = VWolf::rotate(lightInfo.lightMatrix, lightInfo.light.direction.x, { 1.0f, 0.0f, 0.0f });
            lightInfo.lightMatrix = VWolf::rotate(lightInfo.lightMatrix, lightInfo.light.direction.y, { 0.0f, 1.0f, 0.0f });
            lightInfo.lightMatrix = VWolf::rotate(lightInfo.lightMatrix, lightInfo.light.direction.z, { 0.0f, 0.0f, 1.0f });
        }
    }

    void OnDraw() override {
        VWolf::Renderer::Begin(camera);
        VWolf::Renderer::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
        VWolf::Renderer::Clear();
        for (LightInfo& lightInfo: lights) {
            VWolf::Renderer::AddLight(lightInfo.light);
        }
        
        VWolf::Renderer::SetMaterial(material2);
        for(auto gameObject: gameObjects1)
            VWolf::Renderer::DrawMesh(gameObject->GetData(), gameObject->transform.matrix);
//        VWolf::Renderer::SetMaterial(material2);
        for(auto gameObject: gameObjects2)
            VWolf::Renderer::DrawMesh(gameObject->GetData(), gameObject->transform.matrix);
        
        // Light representation
        VWolf::Renderer::SetMaterial(material1);
        for (LightInfo& lightInfo: lights) {
            if (lightInfo.light.type == VWolf::Light::LightType::Point)
                VWolf::Renderer::DrawMesh(pointMesh, lightInfo.lightMatrix);
            else if (lightInfo.light.type == VWolf::Light::LightType::Directional)
                VWolf::Renderer::DrawMesh(directionalMesh, lightInfo.lightMatrix);
            else if (lightInfo.light.type == VWolf::Light::LightType::Spot)
                VWolf::Renderer::DrawMesh(spotMesh, lightInfo.lightMatrix);
        }
        
        VWolf::Renderer::End();
    }

    void OnGUI() override {
        ImGui::NewFrame();
        ImGui::Begin("Shapes");
        for(auto gameObject: gameObjects1) {
            ImGui::PushID(gameObject->GetId());
            ImGui::LabelText("Shape #", "%s", gameObject->GetId());
            ImGui::DragFloat3("Position", VWolf::value_ptr(gameObject->transform.position));
            ImGui::DragFloat3("Rotation", VWolf::value_ptr(gameObject->transform.rotation));
            ImGui::DragFloat3("Scale", VWolf::value_ptr(gameObject->transform.scale));
            ImGui::PopID();
        }
        for(auto gameObject: gameObjects2) {
            ImGui::PushID(gameObject->GetId());
            ImGui::LabelText("Shape #", "%s", gameObject->GetId());
            ImGui::DragFloat3("Position", VWolf::value_ptr(gameObject->transform.position));
            ImGui::DragFloat3("Rotation", VWolf::value_ptr(gameObject->transform.rotation));
            ImGui::DragFloat3("Scale", VWolf::value_ptr(gameObject->transform.scale));
            ImGui::PopID();
        }
        ImGui::End();
        
        ImGui::Begin("Materials");
//        ImGui::PushID("Material 1");
//        ImGui::LabelText("Material 1", "");
//        ImGui::ColorEdit4("Ambient Color", VWolf::value_ptr(mat1->ambientColor));
//        ImGui::ColorEdit4("Diffuse Color", VWolf::value_ptr(mat1->diffuseColor));
//        ImGui::DragFloat3("Specular Vector", VWolf::value_ptr(mat1->specular));
//        ImGui::SliderFloat("Shinines", &mat1->shinines, 0, 1);
//        ImGui::PopID();
        ImGui::PushID("Material");
        ImGui::LabelText("Material", "");
        ImGui::ColorEdit4("Ambient Color", VWolf::value_ptr(mat2->ambientColor));
        ImGui::ColorEdit4("Diffuse Color", VWolf::value_ptr(mat2->diffuseColor));
        ImGui::DragFloat3("Specular Vector", VWolf::value_ptr(mat2->specular), 0.01f, 0, 1);
        ImGui::SliderFloat("Shinines", &mat2->shinines, 0, 180);
        ImGui::PopID();

        int i = 0;
        ImGui::PushID("Light");
        for (LightInfo& lightInfo: lights) {
            
            ImGui::PushID(i);
            ImGui::LabelText("Light", "");
            if (ImGui::RadioButton("Directional", lightInfo.light.type == VWolf::Light::LightType::Directional)) {
                lightInfo.light.type = VWolf::Light::LightType::Directional;
            }
            if (ImGui::RadioButton("Point", lightInfo.light.type == VWolf::Light::LightType::Point)) {
                lightInfo.light.type = VWolf::Light::LightType::Point;
            }
            if (ImGui::RadioButton("Spot", lightInfo.light.type == VWolf::Light::LightType::Spot)) {
                lightInfo.light.type = VWolf::Light::LightType::Spot;
            }
            if (ImGui::RadioButton("Unknown", lightInfo.light.type == VWolf::Light::LightType::Unknown)) {
                lightInfo.light.type = VWolf::Light::LightType::Unknown;
            }
            ImGui::ColorEdit4("Light Color", VWolf::value_ptr(lightInfo.light.color));
            ImGui::DragFloat3("Light Strength", VWolf::value_ptr(lightInfo.light.strength), 0.1f, 0, 1);
            if (lightInfo.light.type == VWolf::Light::LightType::Point)
                ImGui::DragFloat3("Light Position", VWolf::value_ptr(lightInfo.light.position));
            else if (lightInfo.light.type == VWolf::Light::LightType::Directional)
                ImGui::DragFloat3("Light Rotation", VWolf::value_ptr(lightInfo.lightRotation));
            else if (lightInfo.light.type == VWolf::Light::LightType::Spot) {
                ImGui::DragFloat3("Light Position", VWolf::value_ptr(lightInfo.light.position));
                ImGui::DragFloat3("Light Rotation", VWolf::value_ptr(lightInfo.lightRotation));
                ImGui::DragFloat("Light CutOff", &lightInfo.light.cutOff, 0.1f, 0, VWolf::radians(90.0f));
                ImGui::DragFloat("Light Exponent", &lightInfo.light.exponent, 0.1f, 0, 100);
            }
            ImGui::PopID();
            i++;
        }
        ImGui::PopID();
        ImGui::End();
    }

    bool OnWindowResize(VWolf::WindowResizeEvent& e) {
        if (e.GetWidth() != 0 && e.GetHeight() != 0)
            camera->SetViewportSize(e.GetWidth(), e.GetHeight());
        return true;
    }
};

VWOLF_MAIN_APP(RendererSandboxApplication)
