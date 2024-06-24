#pragma once

#include "VWolf/Core/Math/VMath.h"

// TODO: Move this as part of the component system later on.
namespace VWolf {
	class Camera
	{
	public:
        Camera(): m_Projection(perspective(radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip))  {
            UpdateView(Vector3Float(), Quat());
        };
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
        float GetFOV() const { return m_FOV; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetZoom() const { return m_zoom; }
        bool IsOrtographic() const { return m_isOrthographic; }
        // TODO: Remove and use component system
        Vector3Float GetPosition() { return m_position; }
    public:
        // TODO: Will change with transform component
        void UpdateView(Vector3Float position, Quat orientation);
        void UpdateProjection();
        void SetFOV(float fov) { m_FOV = fov; UpdateProjection(); }
        void SetAspectRatio(float ratio) { m_AspectRatio = ratio; UpdateProjection(); }
        void SetNearZ(float nearZ) { m_NearClip = nearZ; UpdateProjection(); }
        void SetFarZ(float farZ) { m_FarClip = farZ; UpdateProjection(); }
        void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }
        void SetZoomLevel(float zoom) { m_zoom = zoom; UpdateProjection(); }
        void SetOrthographic(bool isOrthographic) { m_isOrthographic = isOrthographic; UpdateProjection(); }
    public:
        static Camera* main;
        static void SetMainCamera(Camera* camera) { main = camera; }
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
