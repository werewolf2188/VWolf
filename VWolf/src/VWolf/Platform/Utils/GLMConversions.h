#pragma once

#include "VWolf/Core/Math/Matrices.h"
#include "VWolf/Core/Math/Vectors.h"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace VWolf {
	MatrixFloat3x3 fromGLMMatrix3x3(glm::mat3x3 matrix);

	glm::mat3x3 toGLMMatrix3x3(MatrixFloat3x3 matrix);

	MatrixFloat4x4 fromGLMMatrix4x4(glm::mat4x4 matrix);

	glm::mat4x4 toGLMMatrix4x4(MatrixFloat4x4 matrix);

	Vector2Float fromGLMVector2(glm::vec2 vec2);

	glm::vec2 toGLMVector2(Vector2Float vector);

	Vector3Float fromGLMVector3(glm::vec3 vec3);

	glm::vec3 toGLMVector3(Vector3Float vector);

	Quat fromGLMQuat(glm::quat quat);

	glm::quat toGLMQuat(Quat quat);
}