#pragma once

#include "VWolf/Core/Math/VMath.h"

// TODO: Move this as part of the component system later on.
namespace VWolf {

    // Camera Pass
    struct CameraPass {
        /* TODO: I don't know what this value does
             float cbPerObjectPad1;
         */
        Matrix4x4 view;
        Matrix4x4 invView;
        Matrix4x4 proj;
        Matrix4x4 invProj;
        Matrix4x4 viewProjection;
        Matrix4x4 invViewProjection;
        Vector3 eyePosition;
        float padding;
        Vector2 renderTargetSize;
        Vector2 invRenderTargetSize;
        float nearZ;
        float farZ;
        float totalTime;
        float deltaTime;
    };

	class Camera
	{
	public:
        Camera(): m_Projection(Matrix4x4::Perspective((Mathf::Deg2Rad * m_FOV), m_AspectRatio, m_NearClip, m_FarClip))  {
        };
        Camera(float fov, float aspectRatio, float nearClip, float farClip):
        m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
        m_Projection(Matrix4x4::Perspective((Mathf::Deg2Rad * fov), aspectRatio, nearClip, farClip)) {
        }

		~Camera() = default;
    public:
		const Matrix4x4& GetProjection() const { return m_Projection; }
        Matrix4x4 GetViewProjection(Matrix4x4 viewMatrix) const { return m_Projection * viewMatrix; }
        Vector2 GetDisplaySize() { return { m_ViewportWidth,  m_ViewportHeight }; }
        float GetNearZ() const { return m_NearClip; }
        float GetFarZ() const { return m_FarClip; }
        float GetFOV() const { return m_FOV; }
        float GetAspectRatio() const { return m_AspectRatio; }
        float GetZoom() const { return m_zoom; }
        bool IsOrtographic() const { return m_isOrthographic; }
        VWolf::Vector3 GetFocalPoint() { return m_FocalPoint; }
    public:
        Matrix4x4 CalculateView(Vector3 position, Quaternion orientation);
        void UpdateProjection();
        void SetFOV(float fov) { m_FOV = fov; UpdateProjection(); }
        void SetAspectRatio(float ratio) { m_AspectRatio = ratio; UpdateProjection(); }
        void SetNearZ(float nearZ) { m_NearClip = nearZ; UpdateProjection(); }
        void SetFarZ(float farZ) { m_FarClip = farZ; UpdateProjection(); }
        void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }
        void SetZoomLevel(float zoom) { m_zoom = zoom; UpdateProjection(); }
        void SetOrthographic(bool isOrthographic) { m_isOrthographic = isOrthographic; UpdateProjection(); }
        void SetFocalPoint(VWolf::Vector3 focalPoint) { m_FocalPoint = focalPoint; UpdateProjection(); }
    private:
        Matrix4x4 m_Projection = Matrix4x4::Identity;
        VWolf::Vector3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

        float m_ViewportWidth = 1280.0f;
        float m_ViewportHeight = 720.0f;

        float m_FOV = 45.0f;
        float m_AspectRatio = 1.778f;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;
        float m_zoom = 0;

        bool m_isOrthographic = false;
	};
}
