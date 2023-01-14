#pragma once

#include "VWolf/Core/Math/Math.h"

// TODO: Move this as part of the component system later on.
namespace VWolf {
	class Camera
	{
	public:
		Camera() = default;
        Camera(float fov, float aspectRatio, float nearClip, float farClip):
        m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
        m_Projection(perspective(radians(fov), aspectRatio, nearClip, farClip)) {
            UpdateView(Vector3Float(), Quat());
        }

		~Camera() = default;
    public:
		const MatrixFloat4x4& GetProjection() const { return m_Projection; }
        const MatrixFloat4x4& GetViewMatrix() const { return m_ViewMatrix; }
        MatrixFloat4x4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }
        Vector2Float GetDisplaySize() { return { m_ViewportWidth,  m_ViewportHeight }; }
        float GetNearZ() const { return m_NearClip; }
        float GetFarZ() const { return m_FarClip; }
        // TODO: Remove and use component system
        Vector3Float GetPosition() { return m_position; }
    public:
        // TODO: Will change with transform component
        void UpdateView(Vector3Float position, Quat orientation);
        void UpdateProjection();
        void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }
        void SetZoomLevel(float zoom) { m_zoom = zoom; UpdateProjection(); }
        void SetOrthographic(bool isOrthographic) { m_isOrthographic = isOrthographic; UpdateProjection(); }
    private:
        MatrixFloat4x4 m_Projection = MatrixFloat4x4(1.0);
        MatrixFloat4x4 m_ViewMatrix;

        float m_ViewportWidth = 1280.0f;
        float m_ViewportHeight = 720.0f;

        float m_FOV = 45.0f;
        float m_AspectRatio = 1.778f;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;
        float m_zoom = 0;

        bool m_isOrthographic = false;
        // TODO: Remove and use component system
        Vector3Float m_position;
	};
}