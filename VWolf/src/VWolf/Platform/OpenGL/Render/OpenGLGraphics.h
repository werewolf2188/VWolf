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
        virtual void Initialize() override;
    protected:
        void ClearColorImpl(Color color);
        void ClearImpl();

        virtual void EndProcessingFrame() override;
        virtual void BeginProcessingFrame() override;
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
