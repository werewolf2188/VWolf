#include "vwpch.h"
#include "OpenGLUIManager.h"

#include <imgui.h>
#include "VWolf/Platform/UI/backends/imgui_impl_glfw.h"
#include "VWolf/Platform/UI/backends/imgui_impl_opengl3.h"
#include "VWolf/Platform/UI/backends/imgui_impl_opengl3_loader.h"

namespace VWolf {
	OpenGLUIManager::OpenGLUIManager(GLFWwindow* window): m_window(window)
	{
	}
	void OpenGLUIManager::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// setup platform/renderer bindings
		VWOLF_CORE_ASSERT(ImGui_ImplGlfw_InitForOpenGL(m_window, true), 
			"OpenGLUIManager - Could not initialize ImGUI");
		VWOLF_CORE_ASSERT(ImGui_ImplOpenGL3_Init(),
			"OpenGLUIManager - Could not initialize ImGUI");
	}
	void OpenGLUIManager::Terminate()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}
	void OpenGLUIManager::Render()
	{
		ImGui::Render();
		ImDrawData* data = ImGui::GetDrawData();
		ImGui_ImplOpenGL3_RenderDrawData(data);
	}
	void OpenGLUIManager::NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
	bool OpenGLUIManager::OnEvent(Event& evt) {
		return false;
	}
}
