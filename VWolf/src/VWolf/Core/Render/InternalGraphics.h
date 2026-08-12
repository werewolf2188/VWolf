//
//  InternalGraphics.h
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#pragma once

#include "VWolf/Core/Base.h"
// TODO: Merge camera classes into one
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "Light.h"
#include "Mesh.h"

#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Time.h"

namespace VWolf {
    struct RenderItem;

    class InternalGraphics {
    public:
        virtual ~InternalGraphics() = default;
    public:
        static void SetSingleton(Ref<InternalGraphics> graphics) { graphicsImpl = graphics; }
        static Ref<InternalGraphics> Singleton() { return graphicsImpl; }
    public:
        virtual void SetRenderTexture(Ref<RenderTexture> renderTexture);
        virtual void BeginProcessingFrame() = 0;
    public:
        virtual void EndProcessingFrame();
        virtual void Initialize();
    protected:
        virtual void DrawShadowMap() = 0;
        virtual void DrawQueue() = 0;
        virtual void DrawPostProcess() = 0;
    protected:
        // TODO: Think about how to deal with render textures
        Ref<PRenderTexture> m_p_renderTexture;
        Ref<RenderTexture> renderTexture;
    private:
        static Ref<InternalGraphics> graphicsImpl;
    };
}
