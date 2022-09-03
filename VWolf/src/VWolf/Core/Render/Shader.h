#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/Math.h"

#include "Buffer.h"

#include <functional>

namespace VWolf {
	class Shader {
	public:
		Shader(const std::string& name, BufferLayout layout) : filepath(name), m_layout(layout){};
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string & name, int value) = 0;
		virtual void SetIntArray(const std::string & name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string & name, float value) = 0;
		virtual void SetFloat2(const std::string & name, const Vector2Float & value) = 0;
		virtual void SetFloat3(const std::string & name, const Vector3Float & value) = 0;
		virtual void SetFloat4(const std::string & name, const Vector4Float & value) = 0;
		virtual void SetMat3(const std::string& name, const MatrixFloat3x3& value) = 0;
		virtual void SetMat4(const std::string & name, const MatrixFloat4x4 & value) = 0;

		virtual const std::string& GetName() const = 0;

		void SetLayout(const BufferLayout& layout) { m_layout = layout; };

		static Ref<Shader> Create(const std::string& filepath, BufferLayout layout) {
			return m_create(filepath, layout);
		}
#ifdef VWOLF_CORE
		static void SetDefaultCreateMethod(std::function<Ref<Shader>(const std::string& name, BufferLayout layout)> create) { m_create = create; }
#endif
	protected: 
		const std::string& filepath;
		BufferLayout& m_layout;
	private:
		static std::function<Ref<Shader>(const std::string& name, BufferLayout layout)> m_create;
	};

	inline std::function<Ref<Shader>(const std::string& name, BufferLayout layout)> Shader::m_create = nullptr;
}