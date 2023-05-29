//
//  EditorCamera.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#include "EditorCamera.h"

namespace VWolfPup {
    CameraController::CameraController(VWolf::Ref<VWolf::Camera> camera): camera(camera) {
        
    }

    void CameraController::OnEvent(VWolf::Event& evt) {
        VWolf::Dispatch<VWolf::MouseScrolledEvent>(evt, VWOLF_BIND_EVENT_FN(OnMouseScroll));
    }

    bool CameraController::OnMouseScroll(VWolf::MouseScrolledEvent& e) {
        // Will Zoom
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        camera->UpdateView(CalculatePosition(), GetOrientation());
        return false;
    }

    void CameraController::OnUpdate() {
        // Left shift: Pan
        // Left alt: Rotate
        // Left ctrl: Zoom
        
        if (VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftShift))
        {
            VWolf::Vector2Float delta = GetMouseDelta();
            if (VWolf::Input::IsMouseButtonPressed(VWolf::MouseCode::Left))
                MousePan(delta);
        }
        if (VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftAlt))
        {
            VWolf::Vector2Float delta = GetMouseDelta();
            if (VWolf::Input::IsMouseButtonPressed(VWolf::MouseCode::Left))
                MouseRotate(delta);
        }
        if (VWolf::Input::IsKeyPressed(VWolf::KeyCode::LeftControl))
        {
            VWolf::Vector2Float delta = GetMouseDelta();
            if (VWolf::Input::IsMouseButtonPressed(VWolf::MouseCode::Left))
                MouseZoom(delta.y);
        }

        camera->UpdateView(CalculatePosition(), GetOrientation());
    }

    VWolf::Vector2Float CameraController::GetMouseDelta() {
        const VWolf::Vector2Float& mouse{ VWolf::Input::GetMouseX(), VWolf::Input::GetMouseY() };
        VWolf::Vector2Float delta = (mouse - m_InitialMousePosition) * 0.003f;
        m_InitialMousePosition = mouse;
        return delta;
    }

    void CameraController::SetViewportSize(float width, float height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        camera->SetViewportSize(width, height);
    }

    float CameraController::GetPitch() const { return m_Pitch; }

    float CameraController::GetYaw() const { return m_Yaw; }

    float CameraController::GetDistance() const { return m_Distance; }

    void CameraController::SetDistance(float distance) {
        m_Distance = distance;
        camera->SetZoomLevel(m_Distance);
    }

    void CameraController::MousePan(const VWolf::Vector2Float& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void CameraController::MouseRotate(const VWolf::Vector2Float& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void CameraController::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f)
        {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
        camera->SetZoomLevel(m_Distance);
    }

    std::pair<float, float> CameraController::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float CameraController::RotationSpeed() const
    {
        return 0.8f;
    }

    float CameraController::ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

    VWolf::Vector3Float CameraController::CalculatePosition() const
    {
        return (useDistanceAndFocalPointForPositionCalculation ? m_FocalPoint: VWolf::Vector3Float(0.0f, 0.0f, 0.0f)) -
        GetForwardDirection() * (useDistanceAndFocalPointForPositionCalculation ? m_Distance: 1.0f); //<--- For the skybox, the distance should not be changed
    }

    VWolf::Vector3Float CameraController::GetUpDirection() const
    {
        return VWolf::rotate(GetOrientation(), { 0.0f, 1.0f, 0.0f });
    }

    VWolf::Vector3Float CameraController::GetRightDirection() const
    {
        return VWolf::rotate(GetOrientation(), { 1.0f, 0.0f, 0.0f });
    }

    VWolf::Vector3Float CameraController::GetForwardDirection() const
    {
        return VWolf::rotate(GetOrientation(), { 0.0f, 0.0f, -1.0f });
    }

    VWolf::Quat CameraController::GetOrientation() const {
        return VWolf::Quat({ -m_Pitch, -m_Yaw, 0.0f });
    }
}
