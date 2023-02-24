#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Buffer.h"

#include "VWolf/Platform/DirectX12/Core/DX12Core.h"
#include "VWolf/Platform/DirectX12/Core/DX12Device.h"
#include "VWolf/Platform/DirectX12/Core/DX12Command.h"
#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"

namespace VWolf {
	DirectX12VertexBuffer::DirectX12VertexBuffer(Ref<DX12Device> device, void* vertices, uint32_t size): size(size), data(vertices)
	{
		VWOLF_CORE_ASSERT(vertices);
		VWOLF_CORE_ASSERT(size);
		DX12BufferResourceInfo infoUpload, infoDefault;
		infoUpload.CreateBufferResourceDescription(true);
		infoDefault.CreateBufferResourceDescription(false);

		uploadBuffer = CreateRef<DX12BufferResource>(infoUpload);
		uploadBuffer->CreateBuffer(device, size, vertices);

		defaultBuffer = CreateRef<DX12BufferResource>(infoDefault);
		defaultBuffer->CreateBuffer(device, size);

		VWOLF_CORE_ASSERT(uploadBuffer.get());
		VWOLF_CORE_ASSERT(defaultBuffer.get());
	}

	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
	}

	void DirectX12VertexBuffer::CopyToDefaultBuffer(Ref<DX12Command> commands) const {

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = data;
		subResourceData.RowPitch = size;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		defaultBuffer->TransitionResource(commands,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		//UpdateSubresources<1>(commands->GetCommandList().Get(), defaultBuffer->GetResource().Get(), uploadBuffer->GetResource().Get(), 0, 0, 1, &subResourceData);
		commands->GetCommandList()->CopyResource(defaultBuffer->GetResource().Get(), uploadBuffer->GetResource().Get());
		defaultBuffer->TransitionResource(commands,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void DirectX12VertexBuffer::Bind(Ref<DX12Command> commands) const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = defaultBuffer->GetResource()->GetGPUVirtualAddress();
		vbv.StrideInBytes = MeshData::Layout.GetStride();
		vbv.SizeInBytes = size;

		commands->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);
	}

	DirectX12IndexBuffer::DirectX12IndexBuffer(Ref<DX12Device> device, UINT32* indices, uint32_t count) : size(count * sizeof(uint32_t)), data(indices)
	{
		VWOLF_CORE_ASSERT(indices);
		VWOLF_CORE_ASSERT(count);
		DX12BufferResourceInfo infoUpload, infoDefault;
		infoUpload.CreateBufferResourceDescription(true);
		infoDefault.CreateBufferResourceDescription(false);

		uploadBuffer = CreateRef<DX12BufferResource>(infoUpload);
		uploadBuffer->CreateBuffer(device, size, indices);

		defaultBuffer = CreateRef<DX12BufferResource>(infoDefault);
		defaultBuffer->CreateBuffer(device, size);

		VWOLF_CORE_ASSERT(uploadBuffer.get());
		VWOLF_CORE_ASSERT(defaultBuffer.get());
	}

	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
	}

	void DirectX12IndexBuffer::CopyToDefaultBuffer(Ref<DX12Command> commands) const {
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = data;
		subResourceData.RowPitch = size;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		defaultBuffer->TransitionResource(commands,
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		//UpdateSubresources<1>(commands->GetCommandList().Get(), defaultBuffer->GetResource().Get(), uploadBuffer->GetResource().Get(), 0, 0, 1, &subResourceData);
		commands->GetCommandList()->CopyResource(defaultBuffer->GetResource().Get(), uploadBuffer->GetResource().Get());
		defaultBuffer->TransitionResource(commands,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void DirectX12IndexBuffer::Bind(Ref<DX12Command> commands) const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = defaultBuffer->GetResource()->GetGPUVirtualAddress();
		ibv.Format = DXGI_FORMAT_R32_UINT;
		ibv.SizeInBytes = size;

		commands->GetCommandList()->IASetIndexBuffer(&ibv);
	}
}
#endif