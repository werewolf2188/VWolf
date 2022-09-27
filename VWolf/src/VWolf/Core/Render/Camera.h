#pragma once

#include "VWolf/Core/Math/Math.h"

namespace VWolf {
	class Camera
	{
	public:
		Camera() = default;
		Camera(const MatrixFloat4x4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		virtual const MatrixFloat4x4& GetProjection() const { return m_Projection; }
	protected:
		MatrixFloat4x4 m_Projection = MatrixFloat4x4(1.0);
	};
}