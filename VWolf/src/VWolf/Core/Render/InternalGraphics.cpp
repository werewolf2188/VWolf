//
//  InternalGraphics.cpp
//  VWolf
//
//  Created by Enrique Moises on 7/25/26.
//

#include "vwpch.h"
#include "InternalGraphics.h"
#include "RenderItem.h"

#include "VWolf/Core/Application.h"
#include "VWolf/Core/Time.h"
#include "VWolf/Core/Events/Event.h"
#include "VWolf/Core/Events/ApplicationEvent.h"

namespace VWolf {
    Ref<InternalGraphics> InternalGraphics::graphicsImpl = nullptr;

    void InternalGraphics::Initialize() {
        renderTexture = CreateRef<RenderTexture>(
                                                 Application::GetApplication()->GetWindow()->GetWidth(),
                                                 Application::GetApplication()->GetWindow()->GetHeight()
                                                 );
        m_p_renderTexture = renderTexture->GetInnerTexture();
    }

    void InternalGraphics::SetRenderTexture(Ref<RenderTexture> renderTexture) {
        if (renderTexture != nullptr) {
            this->renderTexture = renderTexture;
            this->m_p_renderTexture = renderTexture->GetInnerTexture();
        } else {
            this->renderTexture = CreateRef<RenderTexture>(
                                                           Application::GetApplication()->GetWindow()->GetWidth(),
                                                           Application::GetApplication()->GetWindow()->GetHeight()
                                                           );
            this->m_p_renderTexture = renderTexture->GetInnerTexture();
        }
    }

    void InternalGraphics::EndProcessingFrame() {
        EventQueue::DefaultQueue->Queue(CreateRef<AppRenderEvent>(Time::GetTotalFrames(), renderTexture));
    }
}
