#pragma once

#include "VWolf/Core/Base.h"

namespace VWolf {
	struct Vector4Float {
		union {
			struct {
				float x, y, z, w;
			};
			struct {
				float r, g, b, a;
			};
		};	

		inline std::string ToString() const
		{
			std::stringstream ss;
			ss << "Vector 4 (x:" << (*this).x << ", y:" << (*this).y << ", z:" << (*this).z << ", w:" << (*this).w << ")";
			return ss.str();
		}

	};
	typedef Vector4Float Color;

	inline std::ostream& operator<<(std::ostream& os, const Vector4Float& v)
	{
		return os << "Vector 4 (x:" << v.x << ", y:" << v.y << ", z:" << v.z << ", w:" << v.w << ")";
	}

	struct Vector3Float {
		union {
			struct {
				float x, y, z;
			};
			struct {
				float r, g, b;
			};
		};
		Vector3Float& operator+=(const Vector3Float& rhs);
		Vector3Float operator-();
		inline std::string ToString() const
		{
			std::stringstream ss;
			ss << "Vector 3 (x:" << (*this).x << ", y:" << (*this).y << ", z:" << (*this).z << ")";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector3Float& v)
	{
		return os << "Vector 3 (x:" << v.x << ", y:" << v.y << ", z:" << v.z << ")";
	}

	struct Vector2Float {
		float x, y;
		inline std::string ToString() const
		{
			std::stringstream ss;
			ss << "Vector 2 (x:" << (*this).x << ", y:" << (*this).y << ")";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector2Float& v)
	{
		return os << "Vector 2 (x:" << v.x << ", y:" << v.y << ")";
	}

	struct Vector4Int {
		union {
			struct {
				int x, y, z, w;
			};
			struct {
				int r, g, b, a;
			};
		};

		inline std::string ToString() const
		{
			std::stringstream ss;
			ss << "Vector 4 (x:" << (*this).x << ", y:" << (*this).y << ", z:" << (*this).z << ", w:" << (*this).w << ")";
			return ss.str();
		}

	};

	inline std::ostream& operator<<(std::ostream& os, const Vector4Int& v)
	{
		return os << "Vector 4 (x:" << v.x << ", y:" << v.y << ", z:" << v.z << ", w:" << v.w << ")";
	}

	struct Vector3Int {
		union {
			struct {
				int x, y, z;
			};
			struct {
				int r, g, b;
			};
		};
		/*Vector3Int& operator+=(const Vector3Float& rhs);
		Vector3Int operator-();*/
		inline std::string ToString() const
		{
			std::stringstream ss;
			ss << "Vector 3 (x:" << (*this).x << ", y:" << (*this).y << ", z:" << (*this).z << ")";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector3Int& v)
	{
		return os << "Vector 3 (x:" << v.x << ", y:" << v.y << ", z:" << v.z << ")";
	}

	struct Vector2Int {
		int x, y;
		inline std::string ToString() const
		{
			std::stringstream ss;
			ss << "Vector 2 (x:" << (*this).x << ", y:" << (*this).y << ")";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Vector2Int& v)
	{
		return os << "Vector 2 (x:" << v.x << ", y:" << v.y << ")";
	}

	struct Quat {
		float x, y, z, w;
		inline std::string ToString() const
		{
			std::stringstream ss;
			ss << "Quat (x:" << (*this).x << ", y:" << (*this).y << ", z:" << (*this).z << ", w:" << (*this).w << ")";
			return ss.str();
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Quat& v)
	{
		return os << "Quat (x:" << v.x << ", y:" << v.y << ", z:" << v.z << ", w:" << v.w << ")";
	}
}