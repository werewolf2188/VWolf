//
//  Camera.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/13/23.
//
#include "vwpch.h"
#include "Camera.h"
namespace VWolf {
    Camera* Camera::main = nullptr;

    void Camera::UpdateView(Vector3 position, Quaternion orientation) {
        // TODO: Remove and use component system
        m_position = position;
        m_ViewMatrix = Matrix4x4::Identity.Translate(position) * orientation.ToMatrix4x4();
        m_ViewMatrix = m_ViewMatrix.GetInverse();
    }

    void Camera::UpdateProjection() {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        if (m_isOrthographic) {
            m_Projection = Matrix4x4::Ortho(-m_AspectRatio * m_zoom, m_AspectRatio * m_zoom, -m_zoom, m_zoom, m_NearClip, m_FarClip);
        } else {
            m_Projection = Matrix4x4::Perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
        }
    }
}
