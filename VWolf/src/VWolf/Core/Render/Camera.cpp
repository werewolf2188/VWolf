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

    void Camera::UpdateView(Vector3Float position, Quat orientation) {
        // TODO: Remove and use component system
        m_position = position;
        m_ViewMatrix = translate(MatrixFloat4x4(1.0f), position) * toMat4(orientation);
        m_ViewMatrix = inverse(m_ViewMatrix);
    }

    void Camera::UpdateProjection() {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        if (m_isOrthographic) {
            m_Projection = ortho(-m_AspectRatio * m_zoom, m_AspectRatio * m_zoom, -m_zoom, m_zoom, m_NearClip, m_FarClip);
        } else {
            m_Projection = perspective(radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
        }
    }
}
