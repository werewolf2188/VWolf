#include <iostream>

#include <imgui/imgui.h>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

#include <array>

#ifdef VWOLF_PLATFORM_WINDOWS
# define M_PI           3.14159265358979323846  /* pi TODO: Move later*/
#define DRIVER_TYPE VWolf::DriverType::OpenGL
#else
#define DRIVER_TYPE VWolf::DriverType::OpenGL
#endif

#define SCREENWIDTH 1280.0f
#define SCREENHEIGHT 720.0f

// For testing purposes.
// TODO: The purpose of this controller is to allow the camera to move. The camera movement should be dictated by scripts or
// TODO: by editor
class CameraController {
public:
    CameraController(VWolf::Ref<VWolf::Camera> camera): camera(camera) {
        
    }
    
    void OnEvent(VWolf::Event& evt) {
        VWolf::Dispatch<VWolf::MouseScrolledEvent>(evt, VWOLF_BIND_EVENT_FN(OnMouseScroll));
    }

    bool OnMouseScroll(VWolf::MouseScrolledEvent& e) {
        // Will Zoom
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        camera->UpdateView(CalculatePosition(), GetOrientation());
        return false;
    }

    void OnUpdate() {
        // TODO: Change the input to be more universal. Middle button does not work in laptops
        
        // Left shift: Pan
        // Left alt: Rotate
        // Left ctrl: Zoom
        
        if (VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftShift))
        {
            VWolf::Vector2Float delta = GetMouseDelta();
            if (VWolf::Input::IsMouseButtonPressed(VWolf::MouseCode::Left))
                MousePan(delta);
        }
        if (VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftAlt))
        {
            VWolf::Vector2Float delta = GetMouseDelta();
            if (VWolf::Input::IsMouseButtonPressed(VWolf::MouseCode::Left))
                MouseRotate(delta);
        }
        if (VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftControl))
        {
            VWolf::Vector2Float delta = GetMouseDelta();
            if (VWolf::Input::IsMouseButtonPressed(VWolf::MouseCode::Left))
                MouseZoom(delta.y);
        }

        camera->UpdateView(CalculatePosition(), GetOrientation());
    }

    VWolf::Vector2Float GetMouseDelta() {
        const VWolf::Vector2Float& mouse{ VWolf::Input::GetMouseX(), VWolf::Input::GetMouseY() };
        VWolf::Vector2Float delta = (mouse - m_InitialMousePosition) * 0.003f;
        m_InitialMousePosition = mouse;
        return delta;
    }

    void SetViewportSize(float width, float height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        camera->SetViewportSize(width, height);
    }

    float GetPitch() const { return m_Pitch; }

    float GetYaw() const { return m_Yaw; }

    float GetDistance() const { return m_Distance; }

    void SetDistance(float distance) {
        m_Distance = distance;
        camera->SetZoomLevel(m_Distance);
    }
private:
    void MousePan(const VWolf::Vector2Float& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void MouseRotate(const VWolf::Vector2Float& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f)
        {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
        camera->SetZoomLevel(m_Distance);
    }

    std::pair<float, float> PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float RotationSpeed() const
    {
        return 0.8f;
    }

    float ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

    VWolf::Vector3Float CalculatePosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }
    
    VWolf::Vector3Float GetUpDirection() const
    {
        return VWolf::rotate(GetOrientation(), { 0.0f, 1.0f, 0.0f });
    }

    VWolf::Vector3Float GetRightDirection() const
    {
        return VWolf::rotate(GetOrientation(), { 1.0f, 0.0f, 0.0f });
    }

    VWolf::Vector3Float GetForwardDirection() const
    {
        return VWolf::rotate(GetOrientation(), { 0.0f, 0.0f, -1.0f });
    }

    VWolf::Quat GetOrientation() const {
        return VWolf::Quat({ -m_Pitch, -m_Yaw, 0.0f });
    }
private:
    VWolf::Ref<VWolf::Camera> camera;
    // This is going to change into a transform, once we start implementing components
    // For position
    VWolf::Vector3Float m_Position = { 0.0f, 0.0f, 0.0f };
    // For rotation
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;
    // For zoom
    VWolf::Vector3Float m_FocalPoint = { 0.0f, 0.0f, 0.0f };
    float m_Distance = 10.0f;
    // Window/Image size
    float m_ViewportWidth = 1280.0f;
    float m_ViewportHeight = 720.0f;

    // Input
    VWolf::Vector2Float m_InitialMousePosition = { 0.0f, 0.0f };
};

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
    VWolf::Ref<VWolf::Camera> camera;
    std::vector<VWolf::Ref<GameObject>> gameObjects1, gameObjects2;
    VWolf::OldMaterial<Albedo> material1;
    VWolf::OldMaterial<Albedo> material2;
    VWolf::Material material_1;
    VWolf::Material material_2;
    VWolf::Ref<VWolf::Texture2D> testTexture;
    VWolf::Ref<VWolf::RenderTexture> renderTexture;
//    LightInfo lightInfo;
    std::vector<LightInfo> lights;
    Albedo* mat1;
    Albedo* mat2;

    VWolf::MeshData pointMesh, directionalMesh, spotMesh;

    VWolf::Ref<CameraController> controller;
    
    const char *materialName = "Material";
public:
    RendererSandboxApplication(): Application(DRIVER_TYPE, { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Renderer Sandbox" } ) {
        camera = VWolf::CreateRef<VWolf::Camera>(30.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        controller = VWolf::CreateRef<CameraController>(camera);
        LoadShaderNames(DRIVER_TYPE);
        
        material1 = VWolf::OldMaterial<Albedo>(shaderNames[0].c_str(), materialName);
        mat1 = material1.GetChildObject();
        mat1->ambientColor = { 1.0f, 0.0f, 0.0f, 1.0f };
        mat1->diffuseColor = { 0.2f, 0.3f, 0.5f, 1.0f };
        mat1->specular = { 0.8f, 0.8f, 0.8f };
        mat1->shinines = 20;
        material2 = VWolf::OldMaterial<Albedo>(shaderNames[1].c_str(), materialName);
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
            if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
                VWolf::ShaderLibrary::LoadShader(shaderNames[i].c_str(), { vsFiles[i], psFiles[i] });
            } else {
                VWolf::ShaderLibrary::LoadShader(shaderNames[i].c_str(),  vsFiles[i], { psFiles[i] }, parameters);
            }
            
        }
        if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
//            testTexture = VWolf::Texture::LoadTexture2D(512, 512);
#ifdef VWOLF_PLATFORM_WINDOWS
            testTexture = VWolf::Texture::LoadTexture2D("src/assets/textExample.png");
#else
            testTexture = VWolf::Texture::LoadTexture2D("../../../Sandbox/src/assets/textExample.png");
#endif

            renderTexture = VWolf::Texture::LoadRenderTexture(SCREENWIDTH, SCREENHEIGHT);
            material_1 = VWolf::Material(shaderNames[0].c_str());
            material_2 = VWolf::Material(shaderNames[1].c_str());
            material_2.SetTexture("u_texture", testTexture);
            material_1.SetColor("u_ambientColor", { 1.0f, 1.0f, 1.0f, 1.0f });
            material_1.SetColor("u_diffuseColor", { 1.0f, 1.0f, 1.0f, 1.0f });
            material_1.SetVector3("u_specular", { 0.8f, 0.8f, 0.8f });
            material_1.SetFloat("u_shinines", 20);
            material_2.SetColor("u_ambientColor", { 1.0f, 1.0f, 1.0f, 1.0f });
            material_2.SetColor("u_diffuseColor", {  1.0f, 1.0f, 1.0f, 1.0f });
            material_2.SetVector3("u_specular", { 0.8f, 0.8f, 0.8f });
            material_2.SetFloat("u_shinines", 20);
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
        controller->OnEvent(evt);
        VWolf::Dispatch<VWolf::WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(RendererSandboxApplication::OnWindowResize));
    }

    void OnUpdate() override {
        controller->OnUpdate();
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
        if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
            VWolf::Graphics::Clear();
            VWolf::Graphics::SetRenderTexture(renderTexture);
            VWolf::Graphics::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
            VWolf::Graphics::Clear();
            
            for (LightInfo& lightInfo: lights) {
                VWolf::Graphics::AddLight(lightInfo.light);
            }
            
            for(auto gameObject: gameObjects1)
                VWolf::Graphics::RenderMesh(gameObject->GetData(),
                                            gameObject->transform.matrix,
                                            material_2);
            for(auto gameObject: gameObjects2)
                VWolf::Graphics::RenderMesh(gameObject->GetData(),
                                            gameObject->transform.matrix,
                                            material_2);
            
            for (LightInfo& lightInfo: lights) {
                if (lightInfo.light.type == VWolf::Light::LightType::Point)
                    VWolf::Graphics::RenderMesh(pointMesh, lightInfo.lightMatrix, material_1);
                else if (lightInfo.light.type == VWolf::Light::LightType::Directional)
                    VWolf::Graphics::RenderMesh(directionalMesh, lightInfo.lightMatrix, material_1);
                else if (lightInfo.light.type == VWolf::Light::LightType::Spot)
                    VWolf::Graphics::RenderMesh(spotMesh, lightInfo.lightMatrix, material_1);
            }
//            VWolf::Graphics::DrawGrid();
            VWolf::Graphics::SetRenderTexture(nullptr);
           
        } else {
            VWolf::Renderer::Begin(camera);
            VWolf::Renderer::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
            VWolf::Renderer::Clear();
            for (LightInfo& lightInfo: lights) {
                VWolf::Renderer::AddLight(lightInfo.light);
            }
    //
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
        if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
            ImGui::PushID(material_2.GetName().c_str());
            ImGui::LabelText("Material", "");
            for(auto property: material_2.GetProperties()) {
                switch(property.second) {
                    case VWolf::ShaderDataType::Float4:
                        ImGui::ColorEdit4(property.first.c_str(), VWolf::value_ptr(material_2.GetColor(property.first)));
                        break;
                    case VWolf::ShaderDataType::Float3:
                        ImGui::DragFloat3(property.first.c_str(), VWolf::value_ptr(material_2.GetVector3(property.first)));
                        break;
                    case VWolf::ShaderDataType::Float:
                        ImGui::SliderFloat(property.first.c_str(), &material_2.GetFloat(property.first), 0, 180);
                        break;
                    default: break;
                }
            }
            ImGui::PopID();
        } else {
            ImGui::PushID("Material");
            ImGui::LabelText("Material", "");
            ImGui::ColorEdit4("Ambient Color", VWolf::value_ptr(mat2->ambientColor));
            ImGui::ColorEdit4("Diffuse Color", VWolf::value_ptr(mat2->diffuseColor));
            ImGui::DragFloat3("Specular Vector", VWolf::value_ptr(mat2->specular), 0.01f, 0, 1);
            ImGui::SliderFloat("Shinines", &mat2->shinines, 0, 180);
            ImGui::PopID();
        }

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

        if (DRIVER_TYPE == VWolf::DriverType::OpenGL) {
            ImGui::Begin("Texture");
            ImGui::Image(testTexture->GetHandler(), ImVec2(128, 128));
            ImGui::End();

            ImGui::Begin("Render Texture", nullptr, ImGuiWindowFlags_NoMouseInputs);
            ImGui::Image(renderTexture->GetHandler(), ImVec2(800, 600), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();
        }
    }

    bool OnWindowResize(VWolf::WindowResizeEvent& e) {
        if (e.GetWidth() != 0 && e.GetHeight() != 0)
            controller->SetViewportSize(e.GetWidth(), e.GetHeight());
        return true;
    }
};

VWOLF_MAIN_APP(RendererSandboxApplication)
