#pragma once
#include "VWolf/Core/Base.h"
#include "Shader.h"
#include <functional>

namespace VWolf {
	class UniformBuffer {
	public:
		UniformBuffer(Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding) : m_shader(shader), m_name(name), m_size(size), m_binding(binding) {};
		virtual ~UniformBuffer() {}
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
#ifdef VWOLF_CORE
		static void SetDefaultCreateMethod(std::function<Ref<UniformBuffer>(Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding)> create) { m_create = create; }
#endif
		static Ref<UniformBuffer> Create(Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding) {
			return m_create(shader, name, size, binding);
		}
	protected:
		uint32_t m_size;
		uint32_t m_binding;
		Ref<Shader> m_shader;
		std::string m_name;
	private:
		static std::function<Ref<UniformBuffer>(Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding)> m_create;
	};

	inline std::function<Ref<UniformBuffer>(Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding)> UniformBuffer::m_create;
}