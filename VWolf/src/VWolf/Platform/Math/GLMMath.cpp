#include "vwpch.h"
#include "GLMMath.h"

#include "VWolf/Platform/Utils/GLMConversions.h"

namespace VWolf {

	GLMMath::GLMMath()
	{
	}
	MatrixFloat4x4 GLMMath::Perspective(float fovy, float aspectRatio, float nearClip, float farClip)
	{
		auto matrix = glm::perspective(Radians(fovy), aspectRatio, nearClip, farClip);
		return fromGLMMatrix4x4(matrix);
	}
	float GLMMath::Radians(float angle)
	{
		return glm::radians(angle);
	}
	MatrixFloat4x4 GLMMath::ToMat4(Quat quat)
	{
		return fromGLMMatrix4x4(glm::toMat4(toGLMQuat(quat)));
	}
	Quat GLMMath::ToQuat(Vector3Float vector3)
	{
		return fromGLMQuat(glm::quat(toGLMVector3(vector3)));
	}
	MatrixFloat4x4 GLMMath::Translate(MatrixFloat4x4 matrix, Vector3Float vector)
	{
		return fromGLMMatrix4x4(glm::translate(toGLMMatrix4x4(matrix), toGLMVector3(vector)));
	}
	Vector3Float GLMMath::Rotate(Quat left, Vector3Float right)
	{
		return fromGLMVector3(glm::rotate(toGLMQuat(left), toGLMVector3(right)));
	}
	Vector3Float GLMMath::Add(Vector3Float left, Vector3Float right)
	{
		return fromGLMVector3(toGLMVector3(left) + toGLMVector3(right));
	}
	Vector2Float GLMMath::Substract(Vector2Float left, Vector2Float right)
	{
		return fromGLMVector2(toGLMVector2(left) - toGLMVector2(right));
	}
	Vector3Float GLMMath::Substract(Vector3Float left, Vector3Float right)
	{
		return fromGLMVector3(toGLMVector3(left) - toGLMVector3(right));
	}
	MatrixFloat4x4 GLMMath::Multiply(MatrixFloat4x4 left, MatrixFloat4x4 right)
	{
		return fromGLMMatrix4x4(toGLMMatrix4x4(left) * toGLMMatrix4x4(right));
	}
	Vector2Float GLMMath::Multiply(Vector2Float left, float right)
	{
		return fromGLMVector2(toGLMVector2(left) * right);
	}
	Vector3Float GLMMath::Multiply(Vector3Float left, float right)
	{
		return fromGLMVector3(toGLMVector3(left) * right);
	}
	MatrixFloat4x4 GLMMath::Inverse(MatrixFloat4x4 matrix)
	{
		return fromGLMMatrix4x4(glm::inverse(toGLMMatrix4x4(matrix)));
	}
	Vector3Float GLMMath::Negate(Vector3Float vector)
	{
		return fromGLMVector3(-toGLMVector3(vector));
	}
}