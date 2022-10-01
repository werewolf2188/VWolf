#include <iostream>

#include <imgui/imgui.h>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

#define DRIVER_TYPE VWolf::DriverType::OpenGL

class SandboxApplication : public VWolf::Application {
private:
	VWolf::PerspectiveCamera camera;
	VWolf::Ref<VWolf::BufferGroup> group;
	VWolf::Ref<VWolf::Shader> colorShader;
	VWolf::DriverType driverType = DRIVER_TYPE;
	VWolf::MatrixFloat4x4 projection;
	VWolf::Ref<VWolf::UniformBuffer> m_uniformBuffer = nullptr;
public:
	SandboxApplication(): Application(DRIVER_TYPE, { 1280, 720, "VWolf Sandbox" } ) {
		camera = VWolf::PerspectiveCamera(30.0f, 1.778f, 0.1f, 1000.0f);
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

		std::stringstream ss;
		ss << "src/shaders/";
#ifdef VWOLF_PLATFORM_WINDOWS
		if (driverType == VWolf::DriverType::DirectX12) {
			ss << "hlsl/color";
		}
		else if (driverType == VWolf::DriverType::OpenGL) {
			ss << "glsl/FlatColor";
		}
#else 
		ss << "glsl/FlatColor";
#endif

#ifndef VWOLF_PLATFORM_MACOS
		// Create
		group = VWolf::BufferGroup::Create();

		float vertices[8 * 7] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, +0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
		+0.5f, +0.5f, -0.5f, 0.0f, 0.0f, 0.1f, 1.0f,
		+0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, +0.5f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, +0.5f, +0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
		+0.5f, +0.5f, +0.5f, 0.8f, 0.0f, 1.0f, 1.0f,
		+0.5f, -0.5f, +0.5f, 1.0f, 1.0f, 1.0f, 1.0f
		};

		VWolf::Ref<VWolf::VertexBuffer> vertexBuffer = VWolf::VertexBuffer::Create(vertices, sizeof(vertices));
		VWolf::BufferLayout layout = {
		{ VWolf::ShaderDataType::Float3, "a_Position" },
		{ VWolf::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		group->AddVertexBuffer(vertexBuffer);
		uint32_t indices[36] = {
			// front face
			2, 1, 0,
			3, 2, 0,

			// back face
			5, 6, 4,
			6, 7, 4,

			// left face
			1, 5, 4,
			0, 1, 4,

			// right face
			6, 2, 3,
			7, 6, 3,

			// top face
			6, 5, 1,
			2, 6, 1,

			// bottom face
			3, 0, 4,
			7, 3, 4
		};
		VWolf::Ref<VWolf::IndexBuffer> indexBuffer = VWolf::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		group->SetIndexBuffer(indexBuffer);
		colorShader = VWolf::Shader::Create(ss.str(), layout);
		m_uniformBuffer = VWolf::UniformBuffer::Create(colorShader, "Camera", sizeof(VWolf::MatrixFloat4x4), 0);
#endif
	}

	~SandboxApplication() {
	}

	void OnEvent(VWolf::Event& evt) {		
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

	void OnUpdate() {
		camera.OnUpdate();
		projection = camera.GetProjection();
		VWolf::Renderer::Begin(camera, colorShader);
		VWolf::Renderer::ClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		VWolf::Renderer::Clear();
#ifndef VWOLF_PLATFORM_MACOS
		colorShader->Bind();
		m_uniformBuffer->SetData(&projection, sizeof(VWolf::MatrixFloat4x4), 0);
		colorShader->SetMat4("u_Transform", VWolf::MatrixFloat4x4(1.0));
		VWolf::Renderer::Submit(group, VWolf::MatrixFloat4x4(1.0));
#endif
		VWolf::Renderer::End();
		// TODO: Remove once its documented.
		//VWOLF_CLIENT_DEBUG("Mouse position x: %0.2f, y: %0.2f", VWolf::Input::GetMouseX(), VWolf::Input::GetMouseY());
	}
	void OnUIUpate() {
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
			ImGui::LabelText("Total Frames", "%d", VWolf::Time::GetTotalFrames());
			ImGui::LabelText("FPS", "%0.3f", VWolf::Time::GetFramesPerSecond());
			ImGui::End();
		}
	}

	bool OnWindowResize(VWolf::WindowResizeEvent& e) {
		if (e.GetWidth() != 0 && e.GetHeight() == 0)
			camera.SetViewportSize(e.GetWidth(), e.GetHeight());
		return true;
	}
};

VWOLF_MAIN_APP(SandboxApplication)