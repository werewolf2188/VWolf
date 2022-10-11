//
//  OpenGLRenderer.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once

#include "VWolf/Core/Render/Renderer.h"

#include "GLSLShader.h"
#include "OpenGLVertexArray.h"
#include "OpenGLRenderAPI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
    class OpenGLRenderer: public Renderer {
    public:
        OpenGLRenderer(GLFWwindow* window): m_window(window) {
            m_renderApi = CreateScope<OpenGLRenderAPI>(window);
        }
        virtual ~OpenGLRenderer() override {};
    protected:
        virtual void ProcessItems() override;
    private:
        MatrixFloat4x4 projection;
        GLFWwindow* m_window;
        Scope<OpenGLRenderAPI> m_renderApi;
        std::vector<Ref<OpenGLVertexArray>> vertexArrays;
    };
}
