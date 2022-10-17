#include <iostream>

#include <imgui/imgui.h>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

#include <array>

#ifdef VWOLF_PLATFORM_WINDOWS
#define DRIVER_TYPE VWolf::DriverType::DirectX12
#else
#define DRIVER_TYPE VWolf::DriverType::OpenGL
#endif

#define SCREENWIDTH 1280.0f
#define SCREENHEIGHT 720.0f

#define NUMSHADERS 2
std::array<std::string, NUMSHADERS> shaderNames = { { "rainbow color", "flat color" } };
std::array<VWolf::ShaderSource, NUMSHADERS> vsFiles;
std::array<VWolf::ShaderSource, NUMSHADERS> psFiles;

void LoadShaderNames(VWolf::DriverType driverType) {
#ifdef VWOLF_PLATFORM_WINDOWS
    if (driverType == VWolf::DriverType::DirectX12) {

        vsFiles = { {
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/hlsl/RainbowColor.hlsl" , "VS" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/hlsl/FlatColor.hlsl" , "VS" }
        } };

        psFiles = { {
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/hlsl/RainbowColor.hlsl" , "PS" },
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/hlsl/FlatColor.hlsl" , "PS" }
        } };
    }
    else if (driverType == VWolf::DriverType::OpenGL) {

        vsFiles = { {
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/glsl/RainbowColor.vert.glsl" , "main" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "src/shaders/glsl/FlatColor.glsl" , "main" }
        } };

        psFiles = { {
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/glsl/RainbowColor.frag.glsl" , "main" },
            { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "src/shaders/glsl/FlatColor.glsl" , "main" }
        } };
    }
#else

    vsFiles = { {
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/RainbowColor.vert.glsl" , "main" },
             { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/FlatColor.glsl" , "main" }
        } };

    psFiles = { {
        { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/RainbowColor.frag.glsl" , "main" },
        { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, "../../../Sandbox/src/shaders/glsl/FlatColor.glsl" , "main" }
    } };
#endif
}

class SandboxApplication : public VWolf::Application {
private:
	VWolf::PerspectiveCamera camera;
	VWolf::Ref<VWolf::BufferGroup> group;
	VWolf::Ref<VWolf::Shader> colorShader;
	VWolf::DriverType driverType = DRIVER_TYPE;
	VWolf::MatrixFloat4x4 projection;
    VWolf::MeshData shape;
    VWolf::MatrixFloat4x4 transform;
    VWolf::Vector3Float position;
    VWolf::Vector3Float rotation;
    VWolf::Vector3Float scale;
public:
	SandboxApplication(): Application(DRIVER_TYPE, { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Sandbox" } ) {
		camera = VWolf::PerspectiveCamera(30.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        position = VWolf::Vector3Float(0.0f);
        rotation = VWolf::Vector3Float(0.0f);
        scale = VWolf::Vector3Float(1.0f);
        LoadShaderNames(DRIVER_TYPE);
		for (auto cmd : VWolf::CommandLineArguments::GetArguments()) {
			VWOLF_CLIENT_DEBUG(cmd.c_str());
		}
		VWOLF_CLIENT_ASSERT(GetWindow()->GetWidth() > 0);
		VWOLF_CLIENT_ASSERT(GetWindow()->GetHeight() > 0);

		/*VWOLF_CLIENT_DEBUG(VWolf::File::OpenTextFile("src/shaders/glsl/FlatColor.vert.glsl").c_str());
		VWOLF_CLIENT_DEBUG(VWolf::File::OpenTextFile("src/shaders/glsl/FlatColor.frag.glsl").c_str());
		VWOLF_CLIENT_DEBUG(VWolf::File::OpenTextFile("src/shaders/hlsl/color.hlsl").c_str());*/

		VWOLF_CLIENT_INFO("Initializing VWolf Sandbox");
#if VWOLF_USE_EVENT_QUEUE
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseMovedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseMoveEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseScrolledEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseScrolledEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseButtonPressedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMousePressEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseButtonReleasedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseReleaseEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::KeyPressedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyPressedEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::KeyReleasedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyReleasedEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::KeyTypedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyTypedEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::WindowResizeEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnWindowResize));
#endif
		// Create
		group = VWolf::BufferGroup::Create();
//        shape = VWolf::ShapeHelper::CreateBox(1, 1, 1, 0);
//        shape = VWolf::ShapeHelper::CreateSphere(2, 32, 32);
//        shape = VWolf::ShapeHelper::CreateGeosphere(1, 4);
        shape = VWolf::ShapeHelper::CreateCylinder(1, 1, 3, 32, 8);
//        shape = VWolf::ShapeHelper::CreateGrid(2, 2, 16, 16);
//        shape = VWolf::ShapeHelper::CreateQuad(-1, 1, 2, 2, 0);

		VWolf::Ref<VWolf::VertexBuffer> vertexBuffer = VWolf::VertexBuffer::Create(shape.GetVertices().data(), shape.GetVertices().size() * sizeof(float));
		vertexBuffer->SetLayout(VWolf::MeshData::Layout);
		group->AddVertexBuffer(vertexBuffer);
		VWolf::Ref<VWolf::IndexBuffer> indexBuffer = VWolf::IndexBuffer::Create(shape.indices.data(), shape.indices.size());
		group->SetIndexBuffer(indexBuffer);
        
		colorShader = VWolf::Shader::Create(shaderNames[0].c_str(),
                                            vsFiles[0],
                                            VWolf::MeshData::Layout,
                                            { psFiles[0] },
                                            {
            { "Camera", VWolf::ShaderParameterType::In, 0, sizeof(VWolf::MatrixFloat4x4) },
            { "Object", VWolf::ShaderParameterType::In, 1, sizeof(VWolf::MatrixFloat4x4) }
        },
                                            {});
//		m_uniformBuffer = VWolf::UniformBuffer::Create(colorShader, "Camera", sizeof(VWolf::MatrixFloat4x4), 0);
	}

	~SandboxApplication() {
	}

	void OnEvent(VWolf::Event& evt) override {		
		VWolf::Application::OnEvent(evt);
		camera.OnEvent(evt);
#if !VWOLF_USE_EVENT_QUEUE
		VWolf::Dispatch<VWolf::MouseMovedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseMoveEvent));
		VWolf::Dispatch<VWolf::MouseScrolledEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseScrolledEvent));
		VWolf::Dispatch<VWolf::MouseButtonPressedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMousePressEvent));
		VWolf::Dispatch<VWolf::MouseButtonReleasedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseReleaseEvent));
		VWolf::Dispatch<VWolf::KeyPressedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyPressedEvent));
		VWolf::Dispatch<VWolf::KeyReleasedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyReleasedEvent));
		VWolf::Dispatch<VWolf::KeyTypedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyTypedEvent));
		VWolf::Dispatch<VWolf::WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnWindowResize));
#endif
	}

	bool OnMouseMoveEvent(VWolf::MouseMovedEvent& evt) {
		// VWOLF_CLIENT_DEBUG(evt.ToString());
		return true;
	}

	bool OnMouseScrolledEvent(VWolf::MouseScrolledEvent& evt) {
		// VWOLF_CLIENT_DEBUG(evt.ToString());
		return true;
	}

	bool OnMousePressEvent(VWolf::MouseButtonPressedEvent& evt) {
		// VWOLF_CLIENT_DEBUG(evt.ToString());
		return true;
	}
	bool OnMouseReleaseEvent(VWolf::MouseButtonReleasedEvent& evt) {
		// VWOLF_CLIENT_DEBUG(evt.ToString());
		return true;
	}

	bool OnKeyPressedEvent(VWolf::KeyPressedEvent& evt) {
		// VWOLF_CLIENT_DEBUG(evt.ToString());
		return true;
	}

	bool OnKeyReleasedEvent(VWolf::KeyReleasedEvent& evt) {
		// VWOLF_CLIENT_DEBUG(evt.ToString());
		return true;
	}

	bool OnKeyTypedEvent(VWolf::KeyTypedEvent& evt) {
		// VWOLF_CLIENT_DEBUG(evt.ToString());
		return true;
	}

    void OnUpdate() override {
        camera.OnUpdate();
        transform = VWolf::translate(VWolf::MatrixFloat4x4(1.0f), position);
        transform = VWolf::rotate(transform, VWolf::radians(rotation.x), { 1.0f, 0.0f, 0.0f });
        transform = VWolf::rotate(transform, VWolf::radians(rotation.y), { 0.0f, 1.0f, 0.0f });
        transform = VWolf::rotate(transform, VWolf::radians(rotation.z), { 0.0f, 0.0f, 1.0f });
        transform = VWolf::scale(transform, scale);
    }

	void OnDraw() override {
		projection = camera.GetProjection();
		VWolf::TestRenderer::Begin(camera, colorShader);
		VWolf::TestRenderer::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		VWolf::TestRenderer::Clear();
		colorShader->Bind();
//		m_uniformBuffer->SetData(&projection, sizeof(VWolf::MatrixFloat4x4), 0);
        colorShader->SetData(&projection, "Camera", sizeof(VWolf::MatrixFloat4x4), 0);
		colorShader->SetData(&transform, "Object", sizeof(VWolf::MatrixFloat4x4), 0);
		VWolf::TestRenderer::Submit(group, VWolf::MatrixFloat4x4(1.0));
		VWolf::TestRenderer::End();
		// TODO: Remove once its documented.
		//VWOLF_CLIENT_DEBUG("Mouse position x: %0.2f, y: %0.2f", VWolf::Input::GetMouseX(), VWolf::Input::GetMouseY());
	}
	void OnGUI() override {
		static bool show_demo_window = true;
		static bool show_debug_window_1 = true;
		static bool show_debug_window_2 = true;
		ImGui::NewFrame();
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		if (show_debug_window_1) {
			ImGui::Begin("Debug values", &show_debug_window_1);
			ImGui::LabelText("Delta Time", "%0.3f", VWolf::Time::GetDeltaTime());
			ImGui::LabelText("Total Time", "%0.3f", VWolf::Time::GetTotalTime());
			ImGui::End();
		}
		
		if (show_debug_window_2) {
			ImGui::Begin("Debug values 2", &show_debug_window_2);
			ImGui::LabelText("Total Frames", "%lld", VWolf::Time::GetTotalFrames());
			ImGui::LabelText("FPS", "%0.3f", VWolf::Time::GetFramesPerSecond());
			ImGui::End();
		}
        
        ImGui::Begin("Shape");
        ImGui::DragFloat3("Position", VWolf::value_ptr(position));
        ImGui::DragFloat3("Rotation", VWolf::value_ptr(rotation));
        ImGui::DragFloat3("Scale", VWolf::value_ptr(scale));
        ImGui::End();
	}

	bool OnWindowResize(VWolf::WindowResizeEvent& e) {
		if (e.GetWidth() != 0 && e.GetHeight() != 0)
			camera.SetViewportSize(e.GetWidth(), e.GetHeight());
		return true;
	}
};

// New Renderer test

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

    VWolf::MeshData GetData() {
        return m_data;
    }
private:
    VWolf::MeshData m_data;
    const char* m_id;
};

class RendererSandboxApplication: public VWolf::Application {
public:
    VWolf::Ref<VWolf::PerspectiveCamera> camera;
    std::vector<VWolf::Ref<GameObject>> gameObjects;
    VWolf::Ref<GameObject> cube;
public:
    RendererSandboxApplication(): Application(DRIVER_TYPE, { (int)SCREENWIDTH, (int)SCREENHEIGHT, "VWolf Renderer Sandbox" } ) {
        camera = VWolf::CreateRef<VWolf::PerspectiveCamera>(30.0f, SCREENWIDTH / SCREENHEIGHT, 0.1f, 1000.0f);
        LoadShaderNames(DRIVER_TYPE);

        std::initializer_list<VWolf::ShaderParameter> parameters = {
         { "Camera", VWolf::ShaderParameterType::In, 0, sizeof(VWolf::MatrixFloat4x4) },
         { "Object", VWolf::ShaderParameterType::In, 1, sizeof(VWolf::MatrixFloat4x4) }
        };
        for (int i = 0; i < NUMSHADERS; i++) {
            VWolf::ShaderLibrary::LoadShader(shaderNames[i].c_str(), vsFiles[i], { psFiles[i] }, parameters);
        }
        gameObjects.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateCylinder(1, 1, 3, 32, 8), "0" ));
        gameObjects.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateSphere(2, 32, 32), "1" ));
        gameObjects.push_back(VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateGrid(2, 2, 16, 16), "2" ));
//        cube = VWolf::CreateRef<GameObject>(VWolf::ShapeHelper::CreateBox(1, 1, 1, 0), "2");
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
        for(auto gameObject: gameObjects)
            gameObject->transform.Apply();
//        cube->transform.Apply();
    }

    void OnDraw() override {
        
//        VWolf::Renderer::Begin(camera);
//        VWolf::Renderer::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
//        VWolf::Renderer::Clear();
//        VWolf::Renderer::SetShader("rainbow color");
//        VWolf::Renderer::DrawMesh(cube->GetData(), cube->transform.matrix);
//        VWolf::Renderer::End();
        
        VWolf::Renderer::Begin(camera);
        VWolf::Renderer::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
        VWolf::Renderer::Clear();
        VWolf::Renderer::SetShader(shaderNames[1].c_str());
        for(auto gameObject: gameObjects)
            VWolf::Renderer::DrawMesh(gameObject->GetData(), gameObject->transform.matrix);
        VWolf::Renderer::End();
    }

    void OnGUI() override {
        ImGui::NewFrame();
        ImGui::Begin("Shapes");
        for(auto gameObject: gameObjects) {
            ImGui::PushID(gameObject->GetId());
            ImGui::LabelText("Shape #", "%s", gameObject->GetId());
            ImGui::DragFloat3("Position", VWolf::value_ptr(gameObject->transform.position));
            ImGui::DragFloat3("Rotation", VWolf::value_ptr(gameObject->transform.rotation));
            ImGui::DragFloat3("Scale", VWolf::value_ptr(gameObject->transform.scale));
            ImGui::PopID();
        }
        ImGui::End();

//        ImGui::Begin("Cube");
//        ImGui::LabelText("Cube #", "%s", cube->GetId());
//        ImGui::DragFloat3("Position", VWolf::value_ptr(cube->transform.position));
//        ImGui::DragFloat3("Rotation", VWolf::value_ptr(cube->transform.rotation));
//        ImGui::DragFloat3("Scale", VWolf::value_ptr(cube->transform.scale));
//        ImGui::End();
    }

    bool OnWindowResize(VWolf::WindowResizeEvent& e) {
        if (e.GetWidth() != 0 && e.GetHeight() != 0)
            camera->SetViewportSize(e.GetWidth(), e.GetHeight());
        return true;
    }
};

VWOLF_MAIN_APP(RendererSandboxApplication)
