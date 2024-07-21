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
        SceneViewer(VWolf::Ref<VWolf::Camera> camera, VWolf::DriverType driverType, uint32_t width, uint32_t height);
        ~SceneViewer();
    public:
        void OnGui() override;
        VWolf::Ref<VWolf::RenderTexture> GetRenderTexture() { return renderTexture; }
        void SetSelectedObject(VWolf::Ref<VWolf::GameObject> selectedObject) { this->selectedObject = selectedObject; }
        void SetPlaying(bool isPlay) { isPlaying = isPlay; }
        bool IsHovering() { return isHovering; }
    protected:
        virtual void SetInContainer() override;
    private:
        VWolf::Ref<VWolf::RenderTexture> renderTexture;
        VWolf::DriverType driverType;
        uint32_t operation = 7; // ImGuizmo::OPERATION::TRANSLATE
        VWolf::Ref<VWolf::GameObject> selectedObject;
        VWolf::Ref<VWolf::Camera> camera;
        bool isHovering = false, isPlaying = false;
    };
}
