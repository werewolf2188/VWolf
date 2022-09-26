#include "vwpch.h"
#include "DirectX12ConstantBuffer.h"

namespace VWolf {

	static MatrixFloat4x4 matrix(
		41.0274, 0, 0, 0,
		0, 72.9467, 0, 0,
		0, 0, -1.0002, -1,
		0, 0, 49.81, 50);

	struct ConstantBufferContext
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mMappedData = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
	};

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}


	DirectX12ConstantBuffer::DirectX12ConstantBuffer(HWND__* window, DirectX12Context* context, Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding) :
		m_window(window), m_context(context), UniformBuffer(shader, name, size, binding)
	{
		m_cbContext = CreateScope<ConstantBufferContext>();
		ThrowIfFailed(context->md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(size) * 1),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_cbContext->mUploadBuffer)));

		//CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(m_cbContext->mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_cbContext->mMappedData)));
		//ZeroMemory(m_cbContext->mMappedData, CalcConstantBufferByteSize(size) * 1);
		VWOLF_CORE_ASSERT(m_cbContext->mUploadBuffer);
		VWOLF_CORE_ASSERT(m_cbContext->mMappedData);

		/*UINT objCBByteSize = CalcConstantBufferByteSize(size);
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_cbContext->mUploadBuffer.Get()->GetGPUVirtualAddress();
		int boxCBufIndex = 0;
		cbAddress += boxCBufIndex * objCBByteSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = CalcConstantBufferByteSize(size);

		dx12InitializeDescriptorHeap(context->md3dDevice, m_cbContext->mSrvHeap, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		context->md3dDevice->CreateConstantBufferView(
			&cbvDesc,
			m_cbContext->mSrvHeap->GetCPUDescriptorHandleForHeapStart());*/
	}
	DirectX12ConstantBuffer::~DirectX12ConstantBuffer()
	{
		if (m_cbContext->mUploadBuffer != nullptr)
			m_cbContext->mUploadBuffer->Unmap(0, nullptr);

		m_cbContext->mMappedData = nullptr;
	}
	void DirectX12ConstantBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		memcpy(&m_cbContext->mMappedData[offset * CalcConstantBufferByteSize(size)], data, size);
		// Attach root descriptor directly to the command list
		m_context->mCommandList->SetGraphicsRootConstantBufferView(0, m_cbContext->mUploadBuffer.Get()->GetGPUVirtualAddress());
		// Attach descriptor table via descriptor heap
		/*dx12SetDescriptorHeaps(m_context, m_cbContext->mSrvHeap);
		m_context->mCommandList->SetGraphicsRootDescriptorTable(0, m_cbContext->mSrvHeap->GetGPUDescriptorHandleForHeapStart());*/
	}
}