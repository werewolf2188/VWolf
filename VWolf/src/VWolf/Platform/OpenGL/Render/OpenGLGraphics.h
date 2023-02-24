//
//  OpenGLRenderer.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once

#include "VWolf/Core/Render/Graphics.h"

#include "GLSLShader.h"
#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"
#include "OpenGLTexture.h"

namespace VWolf {
    class OpenGLGraphics: public Graphics {
    public:
        OpenGLGraphics() = default;
        virtual ~OpenGLGraphics() override {};
    public:
        void Build();
    protected:
        virtual void DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera = nullptr) override;
        virtual void RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera = nullptr) override;
        virtual void ClearColorImpl(Color color) override;
        virtual void ClearImpl() override;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light& light) override;
        virtual void DrawGridImpl() override;
        virtual void BeginFrameImpl() override;
        virtual void EndFrameImpl() override;
    private:
        void BindToRenderTexture();
        void UnbindToRenderTexture();
    private:
        // TODO: Plan later
        std::vector<Light> lights;
        Ref<GLSLShader> gridShader;
    };
}