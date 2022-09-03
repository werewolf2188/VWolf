#include "vwpch.h"
#include "GLMConversions.h"

namespace VWolf {

	MatrixFloat3x3 fromGLMMatrix3x3(glm::mat3x3 matrix) {
		auto vector1 = matrix[0];
		auto vector2 = matrix[1];
		auto vector3 = matrix[2];
		return {
			vector1.x, vector1.y, vector1.z,
			vector2.x, vector2.y, vector2.z,
			vector3.x, vector3.y, vector3.z
		};
	}

	glm::mat3x3 toGLMMatrix3x3(MatrixFloat3x3 matrix) {
		return glm::mat3x3(
			matrix.aa, matrix.ab, matrix.ac, 
			matrix.ba, matrix.bb, matrix.bc, 
			matrix.ca, matrix.cb, matrix.cc 
		);
	}

	MatrixFloat4x4 fromGLMMatrix4x4(glm::mat4x4 matrix) {
		auto vector1 = matrix[0];
		auto vector2 = matrix[1];
		auto vector3 = matrix[2];
		auto vector4 = matrix[3];
		return {
			vector1.x, vector1.y, vector1.z, vector1.w,
			vector2.x, vector2.y, vector2.z, vector2.w,
			vector3.x, vector3.y, vector3.z, vector3.w,
			vector4.x, vector4.y, vector4.z, vector4.w
		};
	}

	glm::mat4x4 toGLMMatrix4x4(MatrixFloat4x4 matrix) {
		return glm::mat4x4(
			matrix.aa, matrix.ab, matrix.ac, matrix.ad,
			matrix.ba, matrix.bb, matrix.bc, matrix.bd,
			matrix.ca, matrix.cb, matrix.cc, matrix.cd,
			matrix.da, matrix.db, matrix.dc, matrix.dd
		);
	}

	Vector2Float fromGLMVector2(glm::vec2 vec2) {
		return { vec2.x, vec2.y };
	}

	glm::vec2 toGLMVector2(Vector2Float vector) {
		return glm::vec2(vector.x, vector.y);
	}

	Vector3Float fromGLMVector3(glm::vec3 vec3) {
		return { vec3.x, vec3.y, vec3.z };
	}

	glm::vec3 toGLMVector3(Vector3Float vector) {
		return glm::vec3(vector.x, vector.y, vector.z);
	}

	Quat fromGLMQuat(glm::quat quat) {
		return { quat.x, quat.y, quat.z, quat.w };
	}
	glm::quat toGLMQuat(Quat quat) {
		return glm::quat(quat.w, quat.x, quat.y, quat.z);
	}
}