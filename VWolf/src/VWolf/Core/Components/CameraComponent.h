//
//  CameraComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 6/28/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/Camera.h"
#include "VWolf/Core/Math/VMath.h"

namespace VWolf {
    class TransformComponent;

    class CameraComponent: public Component {
    public:
        CameraComponent();
        CameraComponent(CameraComponent& camera);
        CameraComponent(CameraComponent&& camera);
        ~CameraComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        float& GetViewportWidth() { return m_ViewportWidth; }
        void SetViewportWidth(float viewportWidth) { m_ViewportWidth = viewportWidth; }
        float& GetViewportHeight() { return m_ViewportHeight; }
        void SetViewportHeight(float viewportHeight) { m_ViewportHeight = viewportHeight; }

        float& GetFOV() { return m_FOV; }
        void SetFOV(float fov) { m_FOV = fov; }
        float& GetAspectRatio() { return m_AspectRatio; }
        void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }
        float& GetNearClip() { return m_NearClip; }
        void SetNearClip(float nearClip) { m_NearClip = nearClip; }
        float& GetFarClip() { return m_FarClip; }
        void SetFarClip(float farClip) { m_FarClip = farClip; }
        float& GetZoom() { return m_zoom; }
        void SetZoom(float zoom) { m_zoom = zoom; }

        bool& IsOrthographic() { return m_isOrthographic; }
        void SetOrthographic(bool isOrthographic) { m_isOrthographic = isOrthographic; }
    public:
        Ref<Camera> GetCamera(TransformComponent component);
        Ref<Camera> GetCamera() { return m_camera; }
    private:
        Ref<Camera> m_camera;

        float m_ViewportWidth;
        float m_ViewportHeight;

        float m_FOV;
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;
        float m_zoom;

        bool m_isOrthographic = false;

        VWOLF_COMPONENT_INSPECTOR_DEFINE(CameraComponent);
    };
}
