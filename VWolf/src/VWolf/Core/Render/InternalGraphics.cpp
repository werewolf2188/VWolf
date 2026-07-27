//
//  InternalGraphics.cpp
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#include "vwpch.h"
#include "InternalGraphics.h"
#include "RenderItem.h"

namespace VWolf {
    Ref<InternalGraphics> InternalGraphics::graphicsImpl = nullptr;

    void InternalGraphics::BeginFrame() {
        if (graphicsImpl) {
            graphicsImpl->BeginFrameImpl();
        }
    }

    void InternalGraphics::EndFrame() {
        if (graphicsImpl) {
            graphicsImpl->EndFrameImpl();
        }
    }

    void InternalGraphics::BeginScene() {
        if (graphicsImpl) {
            graphicsImpl->BeginSceneImpl();
        }
    }
    void InternalGraphics::EndScene() {
        if (graphicsImpl) {
            graphicsImpl->EndSceneImpl();
        }
    }
}
