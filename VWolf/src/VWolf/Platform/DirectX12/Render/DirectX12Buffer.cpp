#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Buffer.h"

namespace VWolf {

	struct VertexContext {
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
	};

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer) {

		Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

		// Create the actual default buffer resource.
		ThrowIfFailedWithReturnValue(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.GetAddressOf())), nullptr);

		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		ThrowIfFailedWithReturnValue(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())), nullptr);


		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		// Note: uploadBuffer has to be kept alive after the above function calls because
		// the command list has not been executed yet that performs the actual copy.
		// The caller can Release the uploadBuffer after it knows the copy has been executed.
		return defaultBuffer;
	}

	DirectX12VertexBuffer::DirectX12VertexBuffer(HWND__* window, DirectX12Context* context, uint32_t size): 
		m_window(window), m_context(context), m_size(size)
	{
		m_vContext = CreateScope<VertexContext>();
		VWOLF_CORE_ASSERT(size);
	}
	DirectX12VertexBuffer::DirectX12VertexBuffer(HWND__* window, DirectX12Context* context, float* vertices, uint32_t size): 
		m_window(window), m_context(context), m_vertices(vertices), m_size(size)
	{
		VWOLF_CORE_ASSERT(size);
		m_vContext = CreateScope<VertexContext>();
		ThrowIfFailed(D3DCreateBlob(size, &m_vContext->VertexBufferCPU));
		CopyMemory(m_vContext->VertexBufferCPU->GetBufferPointer(), vertices, size);
		VWOLF_CORE_ASSERT(m_vContext->VertexBufferCPU);

		// Not entirely sure about this
		//dx12ResetCommandListAllocator(context);
		dx12ResetCommandList(context);
		m_vContext->VertexBufferGPU = CreateDefaultBuffer(context->md3dDevice.Get(),
			context->mCommandList.Get(), vertices, size, m_vContext->VertexBufferUploader);
		dx12ExecuteCommands(context);
		dx12Flush(context);
		VWOLF_CORE_ASSERT(m_vContext->VertexBufferGPU);
		VWOLF_CORE_ASSERT(m_vContext->VertexBufferUploader);

		m_vContext->VertexBufferByteSize = size;
	}
	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
	}
	void DirectX12VertexBuffer::Bind() const
	{
	}
	void DirectX12VertexBuffer::Unbind() const
	{
	}
	void DirectX12VertexBuffer::SetData(const void* data, uint32_t size)
	{
	}
	const BufferLayout& DirectX12VertexBuffer::GetLayout() const
	{
		return m_layout;
	}
	void DirectX12VertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_layout = layout;
		m_vContext->VertexByteStride = layout.GetStride();
	}

	D3D12_VERTEX_BUFFER_VIEW DirectX12VertexBuffer::VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = m_vContext->VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = m_vContext->VertexByteStride;
		vbv.SizeInBytes = m_vContext->VertexBufferByteSize;

		return vbv;
	}

	struct IndexContext {
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;
		UINT IndexBufferByteSize = 0;

	};

	DirectX12IndexBuffer::DirectX12IndexBuffer(HWND__* window, DirectX12Context* context, uint32_t* indices, uint32_t count):
		m_window(window), m_context(context), m_indices(indices), m_count(count)
	{
		m_iContext = CreateScope<IndexContext>();
		VWOLF_CORE_ASSERT(count);
		uint32_t size = count * sizeof(uint32_t);
		ThrowIfFailed(D3DCreateBlob(size, &m_iContext->IndexBufferCPU));
		CopyMemory(m_iContext->IndexBufferCPU->GetBufferPointer(), indices, size);
		VWOLF_CORE_ASSERT(m_iContext->IndexBufferCPU);

		// Not entirely sure about this
		//dx12ResetCommandListAllocator(context);
		dx12ResetCommandList(context);
		m_iContext->IndexBufferGPU = CreateDefaultBuffer(context->md3dDevice.Get(),
			context->mCommandList.Get(), indices, size, m_iContext->IndexBufferUploader);
		dx12ExecuteCommands(context);
		dx12Flush(context);
		VWOLF_CORE_ASSERT(m_iContext->IndexBufferGPU);
		VWOLF_CORE_ASSERT(m_iContext->IndexBufferUploader);

		m_iContext->IndexBufferByteSize = size;
	}
	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
	}
	void DirectX12IndexBuffer::Bind() const
	{
	}
	void DirectX12IndexBuffer::Unbind() const
	{
	}
	uint32_t DirectX12IndexBuffer::GetCount() const
	{
		return m_count;
	}
	D3D12_INDEX_BUFFER_VIEW DirectX12IndexBuffer::IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = m_iContext->IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = m_iContext->IndexFormat;
		ibv.SizeInBytes = m_iContext->IndexBufferByteSize;

		return ibv;
	}
}
#endif