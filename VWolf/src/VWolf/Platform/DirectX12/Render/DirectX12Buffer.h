#pragma once

#include "VWolf/Core/Render/RenderStructs.h"

#ifdef VWOLF_PLATFORM_WINDOWS

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {

	struct VertexContext;

	class DX12BufferResource;
	class DX12Device;
	class DX12Command;

	class DirectX12VertexBuffer
	{
	public:
		DirectX12VertexBuffer(Ref<DX12Device> device, void* vertices, uint32_t size);
		~DirectX12VertexBuffer();

		void Bind(Ref<DX12Command> commands) const;
		void CopyToDefaultBuffer(Ref<DX12Command> commands) const;
	private:
		Ref<DX12BufferResource> uploadBuffer;
		Ref<DX12BufferResource> defaultBuffer;
		uint32_t size;
		void* data;
	};

	struct IndexContext;

	class DirectX12IndexBuffer
	{
	public:
		DirectX12IndexBuffer(Ref<DX12Device> device, UINT32* indices, uint32_t count);
		~DirectX12IndexBuffer();

		void Bind(Ref<DX12Command> commands) const;
		void CopyToDefaultBuffer(Ref<DX12Command> commands) const;
	private:
		Ref<DX12BufferResource> uploadBuffer;
		Ref<DX12BufferResource> defaultBuffer;
		uint32_t size;
		void* data;
	};
}
#endif