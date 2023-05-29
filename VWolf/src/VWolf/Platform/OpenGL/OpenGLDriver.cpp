#include "vwpch.h"
#include "OpenGLDriver.h"

#include "VWolf/Platform/OpenGL/Windows/GLFWWindow.h"
#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/OpenGL/UI/OpenGLUIManager.h"

#include "VWolf/Core/Render/Graphics.h"
#include "VWolf/Platform/OpenGL/Render/OpenGLGraphics.h"

#include "VWolf/Core/Time.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 1
#define OPENGL_PROFILE GLFW_OPENGL_CORE_PROFILE

namespace VWolf {
	class GLFWTime : public Time {
	protected:
		virtual float GetTime() override {
			return glfwGetTime();
		}
	};

	void OpenGLDriver::Initialize(InitConfiguration config, WindowEventCallback& callback)
	{
		this->callback = &callback;
		glfwInit();
		glfwSetErrorCallback(GLFWErrorCallback);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        VWOLF_CORE_INFO("Resolution %dx%d", mode->width, mode->height);

		window = CreateRef<GLFWWindow>(config, callback);
		UIManager::SetDefault(CreateRef<OpenGLUIManager>((GLFWwindow*)window->GetNativeWindow()));
        Ref<OpenGLGraphics> graphics = CreateRef<OpenGLGraphics>();
        Graphics::SetGraphicsImpl(graphics);
		Time::SetTimeImplementation(CreateRef<GLFWTime>());

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
		}
		window->Initialize();
#ifdef DEBUG        
        if (OPENGL_MINOR_VERSION > 5) {
            GLThrowIfFailed(glEnable(GL_DEBUG_OUTPUT));
            GLThrowIfFailed(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
            GLThrowIfFailed(glDebugMessageCallback(OpenGLMessageCallback, nullptr));
            GLThrowIfFailed(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE));
        }
#endif
        VWOLF_CORE_INFO("GL Vendor %s", glGetString(GL_VENDOR));
        GLThrowIfFailedNoAction("glGetString(GL_VENDOR)");
        VWOLF_CORE_INFO("GL Render %s", glGetString(GL_RENDERER));
        GLThrowIfFailedNoAction("glGetString(GL_RENDERER)");
        VWOLF_CORE_INFO("GL Version %s", glGetString(GL_VERSION));
        GLThrowIfFailedNoAction("glGetString(GL_VERSION)");
        VWOLF_CORE_INFO("GL Shading Language version %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
        GLThrowIfFailedNoAction("glGetString(GL_SHADING_LANGUAGE_VERSION)");
	}

	void OpenGLDriver::Shutdown()
	{
		window.reset();
		glfwTerminate();
	}

	void OpenGLDriver::OnUpdate() {
		window->OnUpdate();
		glfwSwapBuffers((GLFWwindow*)window->GetNativeWindow());
	}

	void OpenGLDriver::OnEvent(Event& evt) {
		callback->OnEvent(evt);
	}

    void OpenGLDriver::Resize(unsigned int m_Width, unsigned int m_Height) {
#ifdef VWOLF_PLATFORM_MACOS
        int width;
        int height;
        glfwGetFramebufferSize((GLFWwindow*)window->GetNativeWindow(), &width, &height);
        GLThrowIfFailed(glViewport(0, 0, width, height));
#else
        GLThrowIfFailed(glViewport(0, 0, m_Width, m_Height));
#endif
    }
}
