//
//  CameraComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/28/24.
//

#include "vwpch.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

namespace VWolf {

    CameraComponent::CameraComponent(): Component("Camera"),
    m_camera(CreateRef<Camera>()),
    m_ViewportWidth(m_camera->GetDisplaySize().GetX()),  m_ViewportHeight(m_camera->GetDisplaySize().GetY()),
    m_FOV(m_camera->GetFOV()), m_AspectRatio(m_camera->GetAspectRatio()), m_NearClip(m_camera->GetNearZ()), m_FarClip(m_camera->GetFarZ()),
    m_isOrthographic(m_camera->IsOrtographic()), m_zoom(m_camera->GetZoom()) { }

    CameraComponent::CameraComponent(CameraComponent& camera): Component("Camera"), m_camera(CreateRef<Camera>()) {
        this->m_ViewportWidth = camera.m_ViewportWidth;
        this->m_ViewportHeight = camera.m_ViewportHeight;
        this->m_FOV = camera.m_FOV;
        this->m_AspectRatio = camera.m_AspectRatio;
        this->m_NearClip = camera.m_NearClip;
        this->m_FarClip = camera.m_FarClip;
        this->m_isOrthographic = camera.m_isOrthographic;
        this->m_zoom = camera.m_zoom;

        m_camera->SetFOV(m_FOV);
        m_camera->SetNearZ(m_NearClip);
        m_camera->SetFarZ(m_FarClip);
        m_camera->SetAspectRatio(m_AspectRatio);
        m_camera->SetOrthographic(m_isOrthographic);
        m_camera->SetZoomLevel(m_zoom);
        m_camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    CameraComponent::CameraComponent(CameraComponent&& camera): Component("Camera"), m_camera(CreateRef<Camera>()) {
        this->m_ViewportWidth = camera.m_ViewportWidth;
        this->m_ViewportHeight = camera.m_ViewportHeight;
        this->m_FOV = camera.m_FOV;
        this->m_AspectRatio = camera.m_AspectRatio;
        this->m_NearClip = camera.m_NearClip;
        this->m_FarClip = camera.m_FarClip;
        this->m_isOrthographic = camera.m_isOrthographic;
        this->m_zoom = camera.m_zoom;

        m_camera->SetFOV(m_FOV);
        m_camera->SetNearZ(m_NearClip);
        m_camera->SetFarZ(m_FarClip);
        m_camera->SetAspectRatio(m_AspectRatio);
        m_camera->SetOrthographic(m_isOrthographic);
        m_camera->SetZoomLevel(m_zoom);
        m_camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);

        camera.m_ViewportWidth = 0;
        camera.m_ViewportHeight = 0;
        camera.m_FOV = 0;
        camera.m_AspectRatio = 0;
        camera.m_NearClip = 0;
        camera.m_FarClip = 0;
        camera.m_isOrthographic = false;
        camera.m_zoom = 0;
    }

    CameraComponent::~CameraComponent() { }

    void CameraComponent::OnInspector() {
        CameraComponent::componentInspector->OnInspector(this);
    }

    Component* CameraComponent::Copy(entt::entity& handle, entt::registry& registry) {
        CameraComponent& component = registry.emplace<CameraComponent>(handle, *this);
        return &component;
    }

    Ref<Camera> CameraComponent::GetCamera(TransformComponent component) {
        m_camera->UpdateView(component.GetPosition(), 
                             Quaternion::Euler(
                                          radians(component.GetEulerAngles().GetX()),
                                          radians(component.GetEulerAngles().GetY()),
                                          radians(component.GetEulerAngles().GetZ())
                             ));
        m_camera->SetFOV(m_FOV);
        m_camera->SetNearZ(m_NearClip);
        m_camera->SetFarZ(m_FarClip);
        m_camera->SetAspectRatio(m_AspectRatio);
        m_camera->SetOrthographic(m_isOrthographic);
        m_camera->SetZoomLevel(m_zoom);
        m_camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
        return m_camera;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(CameraComponent);
}
