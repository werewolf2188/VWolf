#include "vwpch.h"
#include "PerspectiveCamera.h"

#include "VWolf/Core/Input.h"
#include "VWolf/Core/Events/KeyCodes.h"
#include "VWolf/Core/Events/MouseCode.h"

#include "VWolf/Core/Math/Math.h"

namespace VWolf {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), 
		Camera(perspective(radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}

	void PerspectiveCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = perspective(radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void PerspectiveCamera::UpdateView()
	{
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		m_Position = CalculatePosition();

		auto orientation = GetOrientation();
		m_ViewMatrix = translate(MatrixFloat4x4(1.0f), m_Position) * toMat4(orientation);
		m_ViewMatrix = inverse(m_ViewMatrix);
	}

	std::pair<float, float> PerspectiveCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float PerspectiveCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float PerspectiveCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void PerspectiveCamera::OnUpdate(/*Timestep ts*/)
	{
		if (Input::IsKeyPressed(KeyCode::LeftAlt))
		{
			const Vector2Float& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			Vector2Float delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(MouseCode::Middle))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(MouseCode::Left))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(MouseCode::Right))
				MouseZoom(delta.y);
		}

		UpdateView();
	}

	void PerspectiveCamera::OnEvent(Event& e)
	{
		Dispatch<MouseScrolledEvent>(e, VWOLF_BIND_EVENT_FN(PerspectiveCamera::OnMouseScroll));
	}

	bool PerspectiveCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void PerspectiveCamera::MousePan(const Vector2Float& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void PerspectiveCamera::MouseRotate(const Vector2Float& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void PerspectiveCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	Vector3Float PerspectiveCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	MatrixFloat4x4 PerspectiveCamera::GetViewProjection() const
	{
		return m_Projection * m_ViewMatrix;
	}

	Vector3Float PerspectiveCamera::GetUpDirection() const
	{
		return rotate(GetOrientation(), { 0.0f, 1.0f, 0.0f });
	}

	Vector3Float PerspectiveCamera::GetRightDirection() const
	{
		return rotate(GetOrientation(), { 1.0f, 0.0f, 0.0f });
	}

	Vector3Float PerspectiveCamera::GetForwardDirection() const
	{
		return rotate(GetOrientation(), { 0.0f, 0.0f, -1.0f });
	}

	Quat PerspectiveCamera::GetOrientation() const {
		return Quat({ -m_Pitch, -m_Yaw, 0.0f });
	}
}