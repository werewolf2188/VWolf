#include <iostream>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

class SandboxApplication : public VWolf::Application {
public:
	SandboxApplication(): Application(VWolf::DriverType::DirectX12, { 800, 600, "VWolf Sandbox" } ) { 
		VWOLF_CLIENT_ASSERT(GetWindow()->GetWidth() > 0);
		VWOLF_CLIENT_ASSERT(GetWindow()->GetHeight() > 0);

		VWOLF_CLIENT_INFO("Initializing VWolf Sandbox");
#if VWOLF_USE_EVENT_QUEUE
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseMovedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseMoveEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseScrolledEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseScrolledEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseButtonPressedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMousePressEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::MouseButtonReleasedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseReleaseEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::KeyPressedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyPressedEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::KeyReleasedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyReleasedEvent));
		VWolf::EventQueue::defaultQueue->Subscribe<VWolf::KeyTypedEvent>(VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyTypedEvent));
#endif
	}

	void OnEvent(VWolf::Event& evt) {		
		VWolf::Application::OnEvent(evt);
#if !VWOLF_USE_EVENT_QUEUE
		VWolf::Dispatch<VWolf::MouseMovedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseMoveEvent));
		VWolf::Dispatch<VWolf::MouseScrolledEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseScrolledEvent));
		VWolf::Dispatch<VWolf::MouseButtonPressedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMousePressEvent));
		VWolf::Dispatch<VWolf::MouseButtonReleasedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnMouseReleaseEvent));
		VWolf::Dispatch<VWolf::KeyPressedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyPressedEvent));
		VWolf::Dispatch<VWolf::KeyReleasedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyReleasedEvent));
		VWolf::Dispatch<VWolf::KeyTypedEvent>(evt, VWOLF_BIND_EVENT_FN(SandboxApplication::OnKeyTypedEvent));
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
};

VWOLF_MAIN_APP(SandboxApplication)