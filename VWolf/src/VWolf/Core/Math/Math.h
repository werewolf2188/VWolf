#pragma once

#include "VWolf/Core/Base.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/quaternion.hpp>

namespace VWolf {
	using namespace glm;

	typedef glm::vec4 Vector4Float;

	typedef glm::vec4 Color;

	typedef glm::vec3 Vector3Float;

	typedef glm::vec2 Vector2Float;

	typedef glm::vec4 Vector4Int;

	typedef glm::vec3 Vector3Int;

	typedef glm::vec2 Vector2Int;

	typedef glm::quat Quat;

	typedef glm::mat4x4 MatrixFloat4x4;

	inline std::ostream& operator<<(std::ostream& os, const MatrixFloat4x4& v)
	{
		os << "Matrix 4x4: \n";
		os << "[aa:" << v[0][0] << ", ab:" << v[0][1] << ", ac:" << v[0][2] << ", ad:" << v[0][3] << "] \n";
		os << "[ba:" << v[1][0] << ", bb:" << v[1][1] << ", bc:" << v[1][2] << ", bd:" << v[1][3] << "] \n";
		os << "[ca:" << v[2][0] << ", cb:" << v[2][1] << ", cc:" << v[2][2] << ", cd:" << v[2][3] << "] \n";
		os << "[da:" << v[3][0] << ", db:" << v[3][1] << ", dc:" << v[3][2] << ", dd:" << v[3][3] << "]";
		return os;
	}

	typedef glm::mat3x3 MatrixFloat3x3;

	inline std::ostream& operator<<(std::ostream& os, const MatrixFloat3x3& v)
	{
		os << "Matrix 3x3: \n";
		os << "[aa:" << v[0][0] << ", ab:" << v[0][1] << ", ac:" << v[0][2] << "] \n";
		os << "[ba:" << v[1][0] << ", bb:" << v[1][1] << ", bc:" << v[1][2] << "] \n";
		os << "[ca:" << v[2][0] << ", cb:" << v[2][1] << ", cc:" << v[2][2] << "]";
		return os;
	}

	typedef glm::mat2x2 MatrixFloat2x2;

	inline std::ostream& operator<<(std::ostream& os, const MatrixFloat2x2& v)
	{
		os << "Matrix 2x2: \n";
		os << "[aa:" << v[0][0] << ", ab:" << v[0][1] << "] \n";
		os << "[ba:" << v[1][0] << ", bb:" << v[1][1] << "]";
		return os;
	}
}