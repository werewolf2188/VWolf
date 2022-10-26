#pragma once

#include "Camera.h"

#include "VWolf/Core/Events/Event.h"
#include "VWolf/Core/Events/MouseEvent.h"

#include "VWolf/Core/Math/Math.h"

namespace VWolf {
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera() = default;
		PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(/*Timestep ts*/);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const MatrixFloat4x4& GetViewMatrix() const { return m_ViewMatrix; }
		MatrixFloat4x4 GetViewProjection() const;

		Vector3Float GetUpDirection() const;
		Vector3Float GetRightDirection() const;
		Vector3Float GetForwardDirection() const;
		const Vector3Float& GetPosition() const { return m_Position; }
		Quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
        Vector2Float GetDisplaySize() { return { m_ViewportWidth,  m_ViewportHeight }; }
        float GetNearZ() const { return m_NearClip; }
        float GetFarZ() const { return m_FarClip; }
	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const Vector2Float& delta);
		void MouseRotate(const Vector2Float& delta);
		void MouseZoom(float delta);

		Vector3Float CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		MatrixFloat4x4 m_ViewMatrix;
		Vector3Float m_Position = { 0.0f, 0.0f, 0.0f };
		Vector3Float m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		Vector2Float m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;
	};

}