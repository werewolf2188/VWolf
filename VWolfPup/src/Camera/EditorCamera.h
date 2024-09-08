//
//  EditorCamera.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/11/23.
//

#pragma once
#include <iostream>
#include "VWolf.h"

namespace VWolfPup {
// For testing purposes.
// TODO: The purpose of this controller is to allow the camera to move. The camera movement should be dictated by scripts or
// TODO: by editor
    class CameraController {
    public:
        CameraController(VWolf::Ref<VWolf::Camera> camera);

        void OnEvent(VWolf::Event& evt);
        bool OnMouseScroll(VWolf::MouseScrolledEvent& e);
        void OnUpdate();
        VWolf::Vector2 GetMouseDelta();
        void SetViewportSize(float width, float height);
        float GetPitch() const;
        float GetYaw() const;
        float GetDistance() const;
        void SetDistance(float distance);
        void SetUseDistanceAndFocalForPositionCalculation(bool allow) { useDistanceAndFocalPointForPositionCalculation = allow; }
    public:
        inline void SetPitch(float pitch) { m_Pitch = pitch; }
        inline void SetYaw(float yaw) { m_Yaw = yaw; }
    private:
        void MousePan(const VWolf::Vector2& delta);
        void MouseRotate(const VWolf::Vector2& delta);
        void MouseZoom(float delta);
        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;;
        VWolf::Vector3 GetUpDirection() const;
        VWolf::Vector3 GetRightDirection() const;
        VWolf::Vector3 GetForwardDirection() const;
        VWolf::Quaternion GetOrientation() const;
        VWolf::Vector3 CalculatePosition() const;
    private:
        VWolf::Ref<VWolf::Camera> camera;
        // This is going to change into a transform, once we start implementing components
        // For position
        VWolf::Vector3 m_Position = { 0.0f, 0.0f, 0.0f };
        // For rotation
        float m_Pitch = 0.240f;
        float m_Yaw = -0.451f;
        // For zoom
        VWolf::Vector3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
        float m_Distance = 10.0f;
        // Window/Image size
        float m_ViewportWidth = 1280.0f;
        float m_ViewportHeight = 720.0f;

        // Allow calculation
        bool useDistanceAndFocalPointForPositionCalculation = true;

        // Input
        VWolf::Vector2 m_InitialMousePosition = { 0.0f, 0.0f };
    };
}
