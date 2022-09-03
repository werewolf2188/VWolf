#pragma once

#include "VWolf/Core/Math/Vectors.h"

#include "Camera.h"
#include "Shader.h"
#include "BufferGroup.h"

namespace VWolf {
	class RenderAPI {
	public:
		virtual void Begin(Camera& camera, Ref<Shader> shader) = 0;
		virtual void ClearColor(Color color) = 0;
		virtual void Clear() = 0;
		virtual void End() = 0;
		virtual void Resize(unsigned int m_Width, unsigned int m_Height) = 0;
		virtual void DrawIndexed(const Ref<BufferGroup>& group, uint32_t indexCount) = 0;
	};
}