#pragma once
#include "VWolf/Core/Base.h"

namespace VWolf {
	class DX12Surface;

	class DX12Command {
	public:
		DX12Command(Microsoft::WRL::ComPtr<ID3D12Device8> device,
			UINT numCommandFrames,
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE);
		~DX12Command();
	public:
		void BeginFrame();
		void EndFrame(Ref<DX12Surface> surface);
		void Flush();
		UINT64 GetCompletedFence();

		Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCommandQueue() { return commandQueue; }
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6>& GetCommandList() { return commandList; }

		UINT64 GetCurrentFence() { return currentFence; }
	private:
		class DX12CommandFrame {
		public:
			DX12CommandFrame(Microsoft::WRL::ComPtr<ID3D12Device8> device, D3D12_COMMAND_LIST_TYPE type, UINT index);
			~DX12CommandFrame();
		public:
			ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator.Get(); }
			// TODO: Will wait for the last frame to be rendered.
			void Wait(HANDLE fence_event, Microsoft::WRL::ComPtr<ID3D12Fence> fence);
			void SetFenceValue(UINT64 value) { this->value = value; }
		private:
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
			UINT index;
			UINT64 value = 0;
		};
	private:
		// This should be per application
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
		// This should be per thread
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> commandList;
		// This should be per frame
		std::vector<Ref<DX12CommandFrame>> commandFrames;
		// Current Frame
		UINT currentFrame = 0;

		// Fence
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		// Fence value
		UINT64 currentFence = 0;
		// Fence Event
		HANDLE fence_event;
	};
}