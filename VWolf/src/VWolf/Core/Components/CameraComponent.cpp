//
//  CameraComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/28/24.
//

#include "vwpch.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"

namespace VWolf {

    Ref<CameraComponent> CameraComponent::m_main = nullptr;

    CameraComponent::CameraComponent(): Component(ClassNameCleaner::Current().GetClassName<CameraComponent>()),
    m_camera(CreateRef<Camera>()),
    m_ViewportWidth(m_camera->GetDisplaySize().GetX()),  m_ViewportHeight(m_camera->GetDisplaySize().GetY()),
    m_FOV(m_camera->GetFOV()), m_AspectRatio(m_camera->GetAspectRatio()), m_NearClip(m_camera->GetNearZ()), m_FarClip(m_camera->GetFarZ()),
    m_isOrthographic(m_camera->IsOrtographic()), m_zoom(m_camera->GetZoom()) { }

    CameraComponent::CameraComponent(const CameraComponent& camera):
    Component(ClassNameCleaner::Current().GetClassName<CameraComponent>(), camera.id), m_camera(CreateRef<Camera>()) {
        this->m_ViewportWidth = camera.m_ViewportWidth;
        this->m_ViewportHeight = camera.m_ViewportHeight;
        this->m_FOV = camera.m_FOV;
        this->m_AspectRatio = camera.m_AspectRatio;
        this->m_NearClip = camera.m_NearClip;
        this->m_FarClip = camera.m_FarClip;
        this->m_isOrthographic = camera.m_isOrthographic;
        this->m_zoom = camera.m_zoom;
        this->m_FocalPoint = camera.m_FocalPoint;

        m_camera->SetFOV(m_FOV);
        m_camera->SetNearZ(m_NearClip);
        m_camera->SetFarZ(m_FarClip);
        m_camera->SetAspectRatio(m_AspectRatio);
        m_camera->SetOrthographic(m_isOrthographic);
        m_camera->SetZoomLevel(m_zoom);
        m_camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
        m_camera->SetFocalPoint(m_FocalPoint);
    }

    CameraComponent::CameraComponent(CameraComponent&& camera):
    Component(ClassNameCleaner::Current().GetClassName<CameraComponent>(), camera.id), m_camera(CreateRef<Camera>()) {
        this->m_ViewportWidth = camera.m_ViewportWidth;
        this->m_ViewportHeight = camera.m_ViewportHeight;
        this->m_FOV = camera.m_FOV;
        this->m_AspectRatio = camera.m_AspectRatio;
        this->m_NearClip = camera.m_NearClip;
        this->m_FarClip = camera.m_FarClip;
        this->m_isOrthographic = camera.m_isOrthographic;
        this->m_zoom = camera.m_zoom;
        this->m_FocalPoint = camera.m_FocalPoint;

        m_camera->SetFOV(m_FOV);
        m_camera->SetNearZ(m_NearClip);
        m_camera->SetFarZ(m_FarClip);
        m_camera->SetAspectRatio(m_AspectRatio);
        m_camera->SetOrthographic(m_isOrthographic);
        m_camera->SetZoomLevel(m_zoom);
        m_camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
        m_camera->SetFocalPoint(m_FocalPoint);

        camera.m_ViewportWidth = 0;
        camera.m_ViewportHeight = 0;
        camera.m_FOV = 0;
        camera.m_AspectRatio = 0;
        camera.m_NearClip = 0;
        camera.m_FarClip = 0;
        camera.m_isOrthographic = false;
        camera.m_zoom = 0;
        camera.m_FocalPoint = Vector3::Zero;
    }

    CameraComponent::~CameraComponent() { }

    Ref<Component> CameraComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<CameraComponent> component = CopyComponent<CameraComponent>(handle, registry);
        return component;
    }

    Ref<Camera> CameraComponent::GetCamera() {
        m_camera->SetFOV(m_FOV);
        m_camera->SetNearZ(m_NearClip);
        m_camera->SetFarZ(m_FarClip);
        m_camera->SetAspectRatio(m_AspectRatio);
        m_camera->SetOrthographic(m_isOrthographic);
        m_camera->SetZoomLevel(m_zoom);
        m_camera->SetFocalPoint(m_FocalPoint);
    
        m_camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
        return m_camera;
    }

    void CameraComponent::SetGameObject(Weak<GameObject> gameObject) {
        Component::SetGameObject(gameObject);

        if (Ref<GameObject> go = gameObject.lock()) {
            if ((go->GetFlags() & HideFlags::Editor) == HideFlags::None) {
                CameraComponent::SetMainCamera(UnownedRef<CameraComponent>(this));
            }
        }
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(CameraComponent, "CameraComponent")
}
