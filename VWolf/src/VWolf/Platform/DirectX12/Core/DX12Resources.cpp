#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DX12Core.h"
#include "DX12Resources.h"
#include "DX12Device.h"
#include "DX12Surface.h"
#include "DX12Command.h"

#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

namespace VWolf {
	DX12DescriptorHeap::~DX12DescriptorHeap()
	{
	}
	bool DX12DescriptorHeap::Initialize(UINT capacity, bool isShaderVisible)
	{		
		return Initialize(DirectX12Driver::GetCurrent()->GetDevice()->GetDevice(), capacity, isShaderVisible);
	}
	bool DX12DescriptorHeap::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT capacity, bool isShaderVisible)
	{
		auto lock = make_lock(mutex);
		VWOLF_CORE_ASSERT(capacity && capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);
		VWOLF_CORE_ASSERT(!(type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER && capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE));

		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
			isShaderVisible = false;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
		heapDesc.NumDescriptors = capacity;
		heapDesc.Type = type;
		heapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;

		DXThrowIfFailedWithReturnValue(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(heap.GetAddressOf())), false);

		free_handles = CreateScope<UINT[]>(capacity);
		this->capacity = capacity;
		size = 0;
		for (UINT i = 0; i < capacity; i++) free_handles[i] = i; // ????

		descriptorSize = device->GetDescriptorHandleIncrementSize(type);
		cpuStart = heap->GetCPUDescriptorHandleForHeapStart();
		gpuStart = isShaderVisible ? heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
		return true;
	}
	DX12DescriptorHandle DX12DescriptorHeap::Allocate()
	{
		auto lock = make_lock(mutex);
		VWOLF_CORE_ASSERT(heap);
		VWOLF_CORE_ASSERT(size < capacity);

		const UINT index = free_handles[size];
		const UINT offset = index * descriptorSize;
		size++;

		DX12DescriptorHandle handle;
		handle.SetCPUAddress(cpuStart.ptr + offset);
		handle.SetGPUAddress(isShaderVisible() ? gpuStart.ptr + offset: 0);
#if defined(_DEBUG)
		handle.container = this;
		handle.index = index;
#endif
		return handle;
	}
	void DX12DescriptorHeap::Free(DX12DescriptorHandle& handle)
	{
		if (!handle.isValid()) return;
		auto lock = make_lock(mutex);
		VWOLF_CORE_ASSERT(heap && size);
#if defined(_DEBUG)
		VWOLF_CORE_ASSERT(handle.container == this);
		VWOLF_CORE_ASSERT(handle.index < capacity);
#endif
		VWOLF_CORE_ASSERT(handle.GetCPUAddress().ptr >= cpuStart.ptr);
		VWOLF_CORE_ASSERT((handle.GetCPUAddress().ptr - cpuStart.ptr) % descriptorSize == 0);
		const UINT index = (UINT)((handle.GetCPUAddress().ptr - cpuStart.ptr) / descriptorSize);
#if defined(_DEBUG)
		VWOLF_CORE_ASSERT(handle.index == index);
#endif
		handle = {};
	}

	void DX12Resource::TransitionResource(Ref<DX12Command> commands, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subresource, D3D12_RESOURCE_BARRIER_FLAGS flags)
	{
		D3D12_RESOURCE_BARRIER result;
		result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		result.Flags = flags;
		result.Transition.pResource = resource.Get();
		result.Transition.StateBefore = before;
		result.Transition.StateAfter = after;
		result.Transition.Subresource = subresource;
	
		commands->GetCommandList()->ResourceBarrier(1, &result);
		this->current = after;
	}

	DX12TextureResource::DX12TextureResource(DXGI_FORMAT format): format(format)
	{
	}

	DX12TextureResource::~DX12TextureResource()
	{
	}

	void DX12TextureResource::CreateSurfaceRenderTargetResource(UINT index, DX12Surface* surface, Ref<DX12Device> device, Ref<DX12DescriptorHeap> heap)
	{
		if (handle.GetCPUAddress().ptr == 0)
			handle = heap->Allocate();
		
		DXThrowIfFailed(surface->GetSwapChain()->GetBuffer(index, IID_PPV_ARGS(&resource)));
		device->GetDevice()->CreateRenderTargetView(resource.Get(), nullptr, handle.GetCPUAddress());
	}

	void DX12TextureResource::CreateTextureResource(DX12TextureResourceInfo info, Ref<DX12Device> device, Ref<DX12DescriptorHeap> heap)
	{
		D3D12_CLEAR_VALUE* const clearValue{
			(info.newResourceDescription &&
			(info.newResourceDescription->Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET ||
			 info.newResourceDescription->Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)) ?
			&info.newResourceClearValue : nullptr
		};

		if (info.resource) {
			resource = info.resource;
		}
		else if (info.existingHeap && info.newResourceDescription) {
			VWOLF_CORE_ASSERT(!info.resource);
			DXThrowIfFailed(device->GetDevice()->CreatePlacedResource(info.existingHeap, info.allocationInfo.Offset,
				info.newResourceDescription, info.newResourceState, clearValue, IID_PPV_ARGS(&resource)));
		}
		else if (info.newResourceDescription) {
			VWOLF_CORE_ASSERT(!info.resource);
			
			DXThrowIfFailed(device->GetDevice()->CreateCommittedResource(&Heaps.DefaultHeap, D3D12_HEAP_FLAG_NONE,
				info.newResourceDescription, info.newResourceState, clearValue, IID_PPV_ARGS(&resource)));
		}
		VWOLF_CORE_ASSERT(resource);
		
		// Create shader resource view?????
		if (info.shaderResourceDescription) {
			handle = heap->Allocate();
			device->GetDevice()->CreateShaderResourceView(resource.Get(), info.shaderResourceDescription, handle.GetCPUAddress());
		}		
		current = info.newResourceState;
	}

	void DX12TextureResourceInfo::CreateDepthStencilInformation(Ref<DX12Device> device, UINT width, UINT height)
	{
		newResourceDescription = new D3D12_RESOURCE_DESC;
		newResourceDescription->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		newResourceDescription->Alignment = 0;
		newResourceDescription->Width = width;
		newResourceDescription->Height = height;
		newResourceDescription->DepthOrArraySize = 1;
		newResourceDescription->MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		newResourceDescription->Format = DXGI_FORMAT_R24G8_TYPELESS;

		newResourceDescription->SampleDesc.Count = 1; // device->GetMSAAQuality() ? 4 : 1;
		newResourceDescription->SampleDesc.Quality = 0; // device->GetMSAAQuality() ? (device->GetMSAAQuality() - 1) : 0;
		newResourceDescription->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		newResourceDescription->Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		newResourceClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		newResourceClearValue.DepthStencil.Depth = 1.0f;
		newResourceClearValue.DepthStencil.Stencil = 0;

		newResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE; //D3D12_RESOURCE_STATE_COMMON; // Why do I have to start it as common if I'm, not reading it?
	}

	void DX12TextureResourceInfo::CreateRenderTargetInformation(Ref<DX12Device> device, UINT width, UINT height) {
		newResourceDescription = new D3D12_RESOURCE_DESC;
		newResourceDescription->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		newResourceDescription->Alignment = 0;
		newResourceDescription->Width = width;
		newResourceDescription->Height = height;
		newResourceDescription->DepthOrArraySize = 1;
		newResourceDescription->MipLevels = 1;
		newResourceDescription->Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		newResourceDescription->SampleDesc.Count = 1; // device->GetMSAAQuality() ? 4 : 1;
		newResourceDescription->SampleDesc.Quality = 0; // device->GetMSAAQuality() ? (device->GetMSAAQuality() - 1) : 0;
		newResourceDescription->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		newResourceDescription->Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		newResourceClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		newResourceClearValue.Color[0] = 0.0f;
		newResourceClearValue.Color[1] = 0.0f;
		newResourceClearValue.Color[2] = 0.0f;
		newResourceClearValue.Color[3] = 1.0f;

		newResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET; //D3D12_RESOURCE_STATE_COMMON; // Why do I have to start it as common if I'm, not reading it?
	}

	DX12DepthBufferResource::~DX12DepthBufferResource()
	{
	}

	void DX12DepthBufferResource::Create(Ref<DX12Device> device, Ref<DX12DescriptorHeap> dsvHeap)
	{
		VWOLF_CORE_ASSERT(info.newResourceDescription);
		if (texture.GetResource().Get())
			texture.GetResource().Reset();
		else {			
			dsvHandle = dsvHeap->Allocate();
		}
		texture.CreateTextureResource(info, device, nullptr);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// info.newResourceDescription->Format; // This should be decided by different formats
		dsvDesc.Texture2D.MipSlice = 0;

		device->GetDevice()->CreateDepthStencilView(texture.GetResource().Get(), &dsvDesc, dsvHandle.GetCPUAddress());
	}

	void DX12DepthBufferResource::CreateWithShaderResource(Ref<DX12Device> device, Ref<DX12DescriptorHeap> dsvHeap, Ref<DX12DescriptorHeap> srvHeap)
	{
		VWOLF_CORE_ASSERT(info.newResourceDescription);
		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceDescription;
		const DXGI_FORMAT dsvFormat = info.newResourceDescription->Format;
		if (info.newResourceDescription->Format == DXGI_FORMAT_D32_FLOAT) {
			info.newResourceDescription->Format = DXGI_FORMAT_R32_TYPELESS;
			shaderResourceDescription.Format = DXGI_FORMAT_R32_FLOAT;
		}
		shaderResourceDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shaderResourceDescription.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shaderResourceDescription.Texture2D.MipLevels = 1;
		shaderResourceDescription.Texture2D.MostDetailedMip = 0;
		shaderResourceDescription.Texture2D.PlaneSlice = 0;
		shaderResourceDescription.Texture2D.ResourceMinLODClamp = 0;

		VWOLF_CORE_ASSERT(!info.shaderResourceDescription && !info.resource);
		info.shaderResourceDescription = &shaderResourceDescription;

		if (texture.GetResource().Get())
			texture.GetResource().Reset();
		else {
			dsvHandle = dsvHeap->Allocate();
		}
		texture.CreateTextureResource(info, device, srvHeap);		

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = dsvFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		device->GetDevice()->CreateDepthStencilView(texture.GetResource().Get(), &dsvDesc, dsvHandle.GetCPUAddress());
	}

	void DX12DepthBufferResource::SetSize(UINT width, UINT height)
	{
		info.newResourceDescription->Width = width;
		info.newResourceDescription->Height = height;
	}

	DX12RenderTargetResource::~DX12RenderTargetResource()
	{
	}

	void DX12RenderTargetResource::Create(Ref<DX12Device> device, Ref<DX12DescriptorHeap> heap)
	{
		VWOLF_CORE_ASSERT(info.newResourceDescription);		
		if (texture.GetResource().Get())
			texture.GetResource().Reset();
		else {
			rtvHandle = heap->Allocate();
		}
		texture.CreateTextureResource(info, device, nullptr);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//info.newResourceDescription->Format; // This should be decided by different formats
		rtvDesc.Texture2D.MipSlice = 0;

		device->GetDevice()->CreateRenderTargetView(texture.GetResource().Get(), &rtvDesc, rtvHandle.GetCPUAddress());
	}

	void DX12RenderTargetResource::CreateWithShaderResource(Ref<DX12Device> device, Ref<DX12DescriptorHeap> rtvHeap, Ref<DX12DescriptorHeap> srvHeap)
	{
		VWOLF_CORE_ASSERT(info.newResourceDescription);

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceDescription;
		shaderResourceDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		shaderResourceDescription.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shaderResourceDescription.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shaderResourceDescription.Texture2D.MipLevels = 1;
		shaderResourceDescription.Texture2D.MostDetailedMip = 0;
		shaderResourceDescription.Texture2D.PlaneSlice = 0;
		shaderResourceDescription.Texture2D.ResourceMinLODClamp = 0;
		VWOLF_CORE_ASSERT(!info.shaderResourceDescription && !info.resource);
		info.shaderResourceDescription = &shaderResourceDescription;

		if (texture.GetResource().Get())
			texture.GetResource().Reset();
		else {
			rtvHandle = rtvHeap->Allocate();
		}
		texture.CreateTextureResource(info, device, srvHeap);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //info.newResourceDescription->Format; // This should be decided by different formats
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		device->GetDevice()->CreateRenderTargetView(texture.GetResource().Get(), &rtvDesc, rtvHandle.GetCPUAddress());
	}

	void DX12RenderTargetResource::SetSize(UINT width, UINT height)
	{
		info.newResourceDescription->Width = width;
		info.newResourceDescription->Height = height;
	}

	void DX12BufferResourceInfo::CreateBufferResourceDescription(bool isCPUVisible, D3D12_RESOURCE_FLAGS flags)
	{
		this->isCPUVisible = isCPUVisible;

		newResourceDescription = new D3D12_RESOURCE_DESC;
		newResourceDescription->Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		newResourceDescription->Alignment = 0;
		newResourceDescription->Height = 1;
		newResourceDescription->DepthOrArraySize = 1;
		newResourceDescription->MipLevels = 1;
		newResourceDescription->Format = DXGI_FORMAT_UNKNOWN;
		newResourceDescription->SampleDesc = { 1, 0 };
		newResourceDescription->Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		newResourceDescription->Flags = isCPUVisible ? D3D12_RESOURCE_FLAG_NONE : flags;

		VWOLF_CORE_ASSERT(newResourceDescription->Flags == D3D12_RESOURCE_FLAG_NONE ||
			newResourceDescription->Flags == D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		newResourceState = isCPUVisible ? D3D12_RESOURCE_STATE_GENERIC_READ : newResourceState;
;	}
	DX12BufferResource::~DX12BufferResource()
	{
	}

	void DX12BufferResource::CreateBuffer(Ref<DX12Device> device, size_t size, const void* data)
	{
		current = info.newResourceState;
		if (info.existingHeap) {
			DXThrowIfFailed(device->GetDevice()->CreatePlacedResource(info.existingHeap, info.heapOffset, info.newResourceDescription,
				info.newResourceState, nullptr, IID_PPV_ARGS(resource.GetAddressOf())));
		}
		else {
			info.newResourceDescription->Width = size;
			DXThrowIfFailed(device->GetDevice()->CreateCommittedResource(
				info.isCPUVisible ? &Heaps.UploadHeap: &Heaps.DefaultHeap, 
				D3D12_HEAP_FLAG_NONE,
				info.newResourceDescription,
				info.newResourceState, nullptr, IID_PPV_ARGS(resource.GetAddressOf())));
		}

		if (data) {
			if (info.isCPUVisible) {
				D3D12_RANGE range {};
				void* cpu_address{ nullptr };
				resource->Map(0, &range, reinterpret_cast<void**>(&cpu_address));
				VWOLF_CORE_ASSERT(cpu_address);
				memcpy(cpu_address, data, size);
				resource->Unmap(0, &range);
			}
			else {
				// For UAV. Will deal with it later
			}
		}
		VWOLF_CORE_ASSERT(resource.Get());
	}
	void DX12BufferResource::CreateBuffer(Ref<DX12Device> device, size_t size)
	{
		current = info.newResourceState;
		if (info.existingHeap) {
			DXThrowIfFailed(device->GetDevice()->CreatePlacedResource(info.existingHeap, info.heapOffset, info.newResourceDescription,
				info.newResourceState, nullptr, IID_PPV_ARGS(resource.GetAddressOf())));
		}
		else {
			info.newResourceDescription->Width = size;
			DXThrowIfFailed(device->GetDevice()->CreateCommittedResource(
				info.isCPUVisible ? &Heaps.UploadHeap : &Heaps.DefaultHeap,
				D3D12_HEAP_FLAG_NONE,
				info.newResourceDescription,
				info.newResourceState, nullptr, IID_PPV_ARGS(resource.GetAddressOf())));
		}
	}
}

#endif