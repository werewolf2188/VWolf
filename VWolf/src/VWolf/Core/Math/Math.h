#pragma once

#include "VWolf/Core/Base.h"

#include "Vectors.h"
#include "Matrices.h"

namespace VWolf {
	class Math {
	public:
		virtual MatrixFloat4x4 Perspective(float fovy, float aspectRatio, float nearClip, float farClip) = 0;
		virtual float Radians(float angle) = 0;
		virtual MatrixFloat4x4 ToMat4(Quat quat) = 0;
		virtual Quat ToQuat(Vector3Float vector3) = 0;
		virtual MatrixFloat4x4 Translate(MatrixFloat4x4 matrix, Vector3Float vector) = 0;
		virtual Vector3Float Rotate(Quat left, Vector3Float right) = 0;
		virtual Vector3Float Add(Vector3Float left, Vector3Float right) = 0;
		virtual Vector2Float Substract(Vector2Float left, Vector2Float right) = 0;
		virtual Vector3Float Substract(Vector3Float left, Vector3Float right) = 0;
		virtual MatrixFloat4x4 Multiply(MatrixFloat4x4 left, MatrixFloat4x4 right) = 0;
		virtual Vector2Float Multiply(Vector2Float left, float right) = 0;
		virtual Vector3Float Multiply(Vector3Float left, float right) = 0;
		virtual MatrixFloat4x4 Inverse(MatrixFloat4x4 matrix) = 0;
		virtual Vector3Float Negate(Vector3Float vector) = 0;
	public: 
		static Ref<Math> GetInstance() { return m_instance; };
		static void SetInstance(Ref<Math> instance) { m_instance = instance; }

	private:
		static Ref<Math> m_instance;
	};

	inline Ref<Math> Math::m_instance = nullptr;

	inline MatrixFloat4x4 operator*(MatrixFloat4x4 left, MatrixFloat4x4 right) {
		return Math::GetInstance()->Multiply(left, right);
	}
	inline Vector2Float operator*(Vector2Float left, float right) {
		return Math::GetInstance()->Multiply(left, right);
	}
	inline Vector2Float operator-(Vector2Float left, Vector2Float right) {
		return Math::GetInstance()->Substract(left, right);
	}
	inline Vector3Float operator*(Vector3Float left, float right) {
		return Math::GetInstance()->Multiply(left, right);
	}
	inline Vector3Float operator-(Vector3Float left, Vector3Float right) {
		return Math::GetInstance()->Substract(left, right);
	}
	inline Vector3Float& Vector3Float::operator+=(const Vector3Float& right) {
		*this = Math::GetInstance()->Add(*this, right);
		return *this;
	}

	inline Vector3Float Vector3Float::operator-() {
		return Math::GetInstance()->Negate(*this);
	}
}