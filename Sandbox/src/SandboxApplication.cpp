#include <iostream>

#include <imgui/imgui.h>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

#define DRIVER_TYPE VWolf::DriverType::DirectX12

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
    VWolf::Vector3Float scale;
public:
	SandboxApplication(): Application(DRIVER_TYPE, { 1280, 720, "VWolf Sandbox" } ) {
		camera = VWolf::PerspectiveCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        position = VWolf::Vector3Float(0.0f);
        scale = VWolf::Vector3Float(1.0f);
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

#ifdef VWOLF_PLATFORM_WINDOWS
        std::string vertexShaderFile;
        std::string fragmentShaderFile;
        std::string vertexShaderFunction;
        std::string fragmentShaderFunction;
		if (driverType == VWolf::DriverType::DirectX12) {
            vertexShaderFile = "src/shaders/hlsl/color.hlsl";
            fragmentShaderFile = "src/shaders/hlsl/color.hlsl";
            vertexShaderFunction = "VS";
            fragmentShaderFunction = "PS";
		}
		else if (driverType == VWolf::DriverType::OpenGL) {
            vertexShaderFile = "src/shaders/glsl/FlatColor.vert.glsl";
            fragmentShaderFile = "src/shaders/glsl/FlatColor.frag.glsl";
            vertexShaderFunction = "main";
            fragmentShaderFunction = "main";
		}
#else
        std::string vertexShaderFile = "../../../Sandbox/src/shaders/glsl/FlatColor.vert.glsl";
        std::string fragmentShaderFile = "../../../Sandbox/src/shaders/glsl/FlatColor.frag.glsl";
        std::string vertexShaderFunction = "main";
        std::string fragmentShaderFunction = "main";
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
		VWolf::BufferLayout layout = {
		{ VWolf::ShaderDataType::Float3, "a_Position" },
		{ VWolf::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		group->AddVertexBuffer(vertexBuffer);
		VWolf::Ref<VWolf::IndexBuffer> indexBuffer = VWolf::IndexBuffer::Create(shape.indices.data(), shape.indices.size());
		group->SetIndexBuffer(indexBuffer);
        
		colorShader = VWolf::Shader::Create("flat color",
                                            { VWolf::ShaderType::Vertex, VWolf::ShaderSourceType::File, vertexShaderFile.c_str(), vertexShaderFunction.c_str() },
                                            layout,
                                            { { VWolf::ShaderType::Fragment, VWolf::ShaderSourceType::File, fragmentShaderFile.c_str(), fragmentShaderFunction.c_str() } },
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
        transform = VWolf::scale(VWolf::translate(VWolf::MatrixFloat4x4(1.0f), position), scale);
    }

	void OnDraw() override {
		projection = camera.GetProjection();
		VWolf::Renderer::Begin(camera, colorShader);
		VWolf::Renderer::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		VWolf::Renderer::Clear();
		colorShader->Bind();
//		m_uniformBuffer->SetData(&projection, sizeof(VWolf::MatrixFloat4x4), 0);
        colorShader->SetData(&projection, "Camera", sizeof(VWolf::MatrixFloat4x4), 0);
		colorShader->SetData(&transform, "Object", sizeof(VWolf::MatrixFloat4x4), 0);
		VWolf::Renderer::Submit(group, VWolf::MatrixFloat4x4(1.0));
		VWolf::Renderer::End();
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
        ImGui::DragFloat3("Scale", VWolf::value_ptr(scale));
        ImGui::End();
	}

	bool OnWindowResize(VWolf::WindowResizeEvent& e) {
		if (e.GetWidth() != 0 && e.GetHeight() != 0)
			camera.SetViewportSize(e.GetWidth(), e.GetHeight());
		return true;
	}
};

VWOLF_MAIN_APP(SandboxApplication)
