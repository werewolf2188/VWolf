//
//  OpenGLRenderer.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once

#include "VWolf/Core/Render/Renderer.h"

#include "GLSLShader.h"
#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
    class OpenGLRenderer: public Renderer {
    public:
        OpenGLRenderer(GLFWwindow* window): m_window(window) {}
        virtual ~OpenGLRenderer() override {};
    protected:
        virtual void ProcessItems() override;
    private:
        GLFWwindow* m_window;
    };
}
