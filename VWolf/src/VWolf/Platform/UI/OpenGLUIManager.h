#pragma once

#include "VWolf/Core/UI/UIManager.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLUIManager: public UIManager {
	public:
		OpenGLUIManager(GLFWwindow* window);

		virtual void Initialize() override;
		virtual void Terminate() override;
		virtual void Render() override;
		virtual void NewFrame() override;
	private: 
		GLFWwindow* m_window;
	};
}