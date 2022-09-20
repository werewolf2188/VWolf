#pragma once

#include "VWolf/Core/Math/Math.h"

namespace VWolf {
	class GLMMath : public Math {
	public:
		GLMMath();
		virtual MatrixFloat4x4 Perspective(float fovy, float aspectRatio, float nearClip, float farClip) override;
		virtual float Radians(float angle) override;
		virtual MatrixFloat4x4 ToMat4(Quat quat) override;
		virtual Quat ToQuat(Vector3Float vector3) override;
		virtual MatrixFloat4x4 Translate(MatrixFloat4x4 matrix, Vector3Float vector) override;
		virtual Vector3Float Rotate(Quat left, Vector3Float right) override;
		virtual Vector3Float Add(Vector3Float left, Vector3Float right) override;
		virtual Vector2Float Substract(Vector2Float left, Vector2Float right) override;
		virtual Vector3Float Substract(Vector3Float left, Vector3Float right) override;
		virtual MatrixFloat4x4 Multiply(MatrixFloat4x4 left, MatrixFloat4x4 right) override;
		virtual Vector2Float Multiply(Vector2Float left, float right) override;
		virtual Vector3Float Multiply(Vector3Float left, float right) override;
		virtual MatrixFloat4x4 Inverse(MatrixFloat4x4 matrix) override;
		virtual Vector3Float Negate(Vector3Float vector) override;
	};
}
