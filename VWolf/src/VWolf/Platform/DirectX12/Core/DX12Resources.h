#pragma once

#include "VWolf/Core/Base.h"

namespace VWolf {
	class DX12DescriptorHeap;
	class DX12Surface;
	class DX12Device;
	class DX12Command;

	class DX12DescriptorHandle {
	public:
		DX12DescriptorHandle() {}
		~DX12DescriptorHandle() {}
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUAddress() { return cpu; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUAddress() { return gpu; }

		void SetCPUAddress(SIZE_T address) { cpu.ptr = address; }
		void SetGPUAddress(UINT64 address) { gpu.ptr = address; }

		constexpr bool isValid() { return cpu.ptr != 0; }
		constexpr bool isShaderVisible() { return gpu.ptr != 0; }
#if defined(_DEBUG)
	public:
		friend class DX12DescriptorHeap;
		DX12DescriptorHeap* container;
		UINT index;
#endif
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE cpu {0};
		D3D12_GPU_DESCRIPTOR_HANDLE gpu {0};
	};

	class DX12DescriptorHeap {
	public:
		DX12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type): type(type) {}
		~DX12DescriptorHeap();
	public:
		bool Initialize(UINT capacity, bool isShaderVisible);
		bool Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT capacity, bool isShaderVisible);
		DX12DescriptorHandle Allocate();
		void Free(DX12DescriptorHandle& handle);

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap() const { return heap; }
		D3D12_DESCRIPTOR_HEAP_TYPE GetType() const { return type; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStart() const { return cpuStart; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStart() const { return gpuStart; }
		UINT GetCapacity() const { return capacity; } // Available Size
		UINT GetSize() const { return size; } // Number of allocated descriptors
		UINT GetDescriptorSize() const { return descriptorSize; } // The size in bytes of one descriptor
		constexpr bool isShaderVisible() const { return gpuStart.ptr != 0; }
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuStart;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuStart;
		Scope<UINT[]> free_handles; // ??
		std::mutex mutex;
		UINT capacity; // Available Size
		UINT size; // Number of allocated descriptors
		UINT descriptorSize; // The size in bytes of one descriptor
		D3D12_DESCRIPTOR_HEAP_TYPE type;
	};

	class DX12Resource {
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() { return resource; }

		// TODO: This could be used to groupd resource barriers instead
		// TODO: Transform this into a class that adds resource barriers and executes them in piles
		void TransitionResource(Ref<DX12Command> commands, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after,
			UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
		void Release();
		void Name(const char* name);

		D3D12_RESOURCE_STATES GetCurrentState() const { return current; }
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_RESOURCE_STATES current;
	};

	struct DX12TextureResourceInfo {
		// Copying resource
		Microsoft::WRL::ComPtr<ID3D12Resource> resource {nullptr};

		// Create Placed resource
		ID3D12Heap1* existingHeap {nullptr};
		D3D12_RESOURCE_ALLOCATION_INFO1 allocationInfo;

		// New committed resource
		D3D12_RESOURCE_DESC* newResourceDescription {nullptr};
		D3D12_RESOURCE_STATES newResourceState;
		D3D12_CLEAR_VALUE newResourceClearValue;

		// Should it be a shader resource view
		D3D12_SHADER_RESOURCE_VIEW_DESC* shaderResourceDescription {nullptr};

		void CreateDepthStencilInformation(Ref<DX12Device> device, UINT width, UINT height);
		void CreateRenderTargetInformation(Ref<DX12Device> device, UINT width, UINT height);
	};

	struct DX12BufferResourceInfo {
		// New Placed resource
		ID3D12Heap1* existingHeap{ nullptr };
		UINT64 heapOffset {0};

		// New committed resource
		D3D12_RESOURCE_DESC* newResourceDescription{ nullptr };
		D3D12_RESOURCE_STATES newResourceState { D3D12_RESOURCE_STATE_COMMON };

		bool isCPUVisible{ false };

		void CreateBufferResourceDescription(bool isCPUVisible, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
	};

	class DX12BufferResource : public DX12Resource {
	public:
		DX12BufferResource(DX12BufferResourceInfo& info): info(info) {}
		~DX12BufferResource();
	public:
		void CreateBuffer(Ref<DX12Device> device, size_t size, const void* data);
		void CreateBuffer(Ref<DX12Device> device, size_t size);
	private:
		DX12BufferResourceInfo info;

	};

	class DX12TextureResource : public DX12Resource {
	public:
		DX12TextureResource(DXGI_FORMAT format);
		~DX12TextureResource();
	public:
		DX12DescriptorHandle GetHandle() { return handle; }
	public:
		void CreateSurfaceRenderTargetResource(UINT index, DX12Surface* surface, Ref<DX12Device> device, Ref<DX12DescriptorHeap> heap);
		void CreateTextureResource(DX12TextureResourceInfo info, Ref<DX12Device> device, Ref<DX12DescriptorHeap> heap);
	private:
		DXGI_FORMAT format;
		DX12DescriptorHandle handle; // SRV handle
	};

	class DX12RenderTargetResource {
	public:
		DX12RenderTargetResource(DX12TextureResourceInfo info) : info(info), texture(info.newResourceDescription->Format) {}
		~DX12RenderTargetResource();
	public:
		void Create(Ref<DX12Device> device, Ref<DX12DescriptorHeap> heap);
		void CreateWithShaderResource(Ref<DX12Device> device, Ref<DX12DescriptorHeap> rtvHeap, Ref<DX12DescriptorHeap> srvHeap);

		void SetSize(UINT width, UINT height);
		void Release() { texture.Release(); }
		void Name(const char* name) { texture.Name(name); }
		DX12DescriptorHandle& GetHandle() { return rtvHandle; }
		DX12TextureResource& GetTexture() { return texture; }
	private:
		DX12TextureResourceInfo info;
		DX12TextureResource texture;
		DX12DescriptorHandle rtvHandle;
	};

	class DX12DepthBufferResource {
	public:
		DX12DepthBufferResource(DX12TextureResourceInfo info): info(info), texture(info.newResourceDescription->Format) {}
		~DX12DepthBufferResource();
	public:
		void Create(Ref<DX12Device> device, Ref<DX12DescriptorHeap> heap);
		void CreateWithShaderResource(Ref<DX12Device> device, Ref<DX12DescriptorHeap> dsvHeap, Ref<DX12DescriptorHeap> srvHeap);

		void SetSize(UINT width, UINT height);
		void Name(const char* name) { texture.Name(name); }
		DX12DescriptorHandle& GetHandle() { return dsvHandle; }
		DX12TextureResource& GetTexture() { return texture; }
	private:
		DX12TextureResourceInfo info;
		DX12TextureResource texture;
		DX12DescriptorHandle dsvHandle;
	};
}