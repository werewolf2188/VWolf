//
//  OpenGLRenderer.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once

#include "VWolf/Core/Render/InternalGraphics.h"

#include "HLSLOpenGLShader.h"
#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"
#include "OpenGLTexture.h"

namespace VWolf {
    class OpenGLGraphics: public InternalGraphics {
    public:
        OpenGLGraphics() = default;
        virtual ~OpenGLGraphics() override {};
    public:
        void Initialize();
    protected:
        virtual void DrawMeshImpl(Ref<Mesh> mesh1, Vector4 position, Vector4 rotation, Material& material, Ref<Camera> camera = nullptr) override;
        virtual void RenderMeshImpl(Ref<Mesh> mesh1, Matrix4x4 transform, Material& material, Ref<Camera> camera = nullptr) override;
        virtual void ClearColorImpl(Color color) override;
        virtual void ClearImpl() override;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light& light) override;
        virtual void BeginFrameImpl() override;
        virtual void EndFrameImpl() override;
        virtual void SetRenderTextureImpl(Ref<RenderTexture> renderTexture) override;
        virtual void BeginSceneImpl() override;
        virtual void EndSceneImpl() override;
    protected:
        virtual void DrawShadowMap() override;
        virtual void DrawQueue() override;
        virtual void DrawPostProcess() override;
    private:
        void BindToRenderTexture();
        void UnbindToRenderTexture();
    private:
        bool useRenderTexture = false;
        Ref<OpenGLRenderTexture> shadowMap;
        Ref<OpenGLTexture2D> emptyShadowMap;
    };
}
