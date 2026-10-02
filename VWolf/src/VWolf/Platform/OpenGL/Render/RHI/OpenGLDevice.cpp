//
//  OpenGLDevice.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/2/26.
//

#include "vwpch.h"
#include "OpenGLDevice.h"
#include "OpenGLRenderContext.h"

namespace VWolf::RHI {
    Ref<OpenGLDevice> OpenGLDevice::GetCurrentDevice() {
        return CreateRef<OpenGLDevice>();
    }

    OpenGLDevice::OpenGLDevice() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
        }
        
        VWOLF_CORE_INFO("GL Vendor %s", glGetString(GL_VENDOR));
        GLThrowIfFailedNoAction("glGetString(GL_VENDOR)");
        VWOLF_CORE_INFO("GL Render %s", glGetString(GL_RENDERER));
        GLThrowIfFailedNoAction("glGetString(GL_RENDERER)");
        VWOLF_CORE_INFO("GL Version %s", glGetString(GL_VERSION));
        GLThrowIfFailedNoAction("glGetString(GL_VERSION)");
        VWOLF_CORE_INFO("GL Shading Language version %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
        GLThrowIfFailedNoAction("glGetString(GL_SHADING_LANGUAGE_VERSION)");
        glEnable(GL_FRAMEBUFFER_SRGB);
        GLThrowIfFailedNoAction("glEnable(GL_FRAMEBUFFER_SRGB)");
    }

    Ref<RenderContext> OpenGLDevice::CreateRenderContext(const RenderContextSettings& settings) {
        return CreateRef<OpenGLRenderContext>(settings, shared_from_this());
    }
}
