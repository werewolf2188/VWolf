#pragma once

#include "VWolf/Core/Math/VMath.h"

// TODO: Move this as part of the component system later on.
namespace VWolf {
	class Camera
	{
	public:
        Camera(): m_Projection(Matrix4x4::Perspective((Mathf::Deg2Rad * m_FOV), m_AspectRatio, m_NearClip, m_FarClip))  {
            UpdateView(Vector3(), Quaternion());
        };
        Camera(float fov, float aspectRatio, float nearClip, float farClip):
        m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
        m_Projection(Matrix4x4::Perspective((Mathf::Deg2Rad * fov), aspectRatio, nearClip, farClip)) {
            UpdateView(Vector3(), Quaternion());
        }

		~Camera() = default;
    public:
		const Matrix4x4& GetProjection() const { return m_Projection; }
        const Matrix4x4& GetViewMatrix() const { return m_ViewMatrix; }
        Matrix4x4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }
        Vector2 GetDisplaySize() { return { m_ViewportWidth,  m_ViewportHeight }; }
        float GetNearZ() const { return m_NearClip; }
        float GetFarZ() const { return m_FarClip; }
        float GetFOV() const { return m_FOV; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetZoom() const { return m_zoom; }
        bool IsOrtographic() const { return m_isOrthographic; }
        // TODO: Remove and use component system
        Vector3 GetPosition() { return m_position; }
    public:
        // TODO: Will change with transform component
        void UpdateView(Vector3 position, Quaternion orientation);
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
        Matrix4x4 m_Projection = Matrix4x4::Identity;
        Matrix4x4 m_ViewMatrix;

        float m_ViewportWidth = 1280.0f;
        float m_ViewportHeight = 720.0f;

        float m_FOV = 45.0f;
        float m_AspectRatio = 1.778f;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;
        float m_zoom = 0;

        bool m_isOrthographic = false;
        // TODO: Remove and use component system
        Vector3 m_position;
	};
}
