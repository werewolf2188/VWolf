//
//  SceneViewer.h
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/16/23.
//

#pragma once

#include "UICore.h"
#include "VWolf.h"

namespace VWolfPup {
    class SceneViewer: public View {
    public:
        SceneViewer(VWolf::Ref<VWolf::Camera> camera, VWolf::DriverType driverType);
        ~SceneViewer();
    public:
        void OnGui() override;
        bool IsHovering() { return isHovering; }
    protected:
        virtual void SetInContainer() override;
    private:
        bool OnRenderEditor(VWolf::AppRenderEvent& event);
    private:
        VWolf::Ref<VWolf::RenderTexture> renderTexture;
        VWolf::DriverType driverType;
        uint32_t operation = 7; // ImGuizmo::OPERATION::TRANSLATE
        VWolf::Ref<VWolf::Camera> camera;
        bool isHovering = false;
    };
}
