#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DX12Command.h"
#include "DX12Core.h"
#include "DX12Surface.h"

#include "VWolf/Core/Time.h"

namespace VWolf {
	DX12Command::DX12Command(Microsoft::WRL::ComPtr<ID3D12Device8> device,
		UINT numCommandFrames,
		D3D12_COMMAND_LIST_TYPE type,
		D3D12_COMMAND_QUEUE_FLAGS flags) {
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = type;
		queueDesc.NodeMask = 0;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags = flags;
		DXThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
		DXNAME_D3D12_OBJECT(commandQueue, "DirectX 12 Object name: VWolf DirectX 12 Command Queue.");

		commandFrames = std::vector<Ref<DX12CommandFrame>>(numCommandFrames);

		for (UINT i = 0; i < numCommandFrames; i++)
			commandFrames[i] = CreateRef<DX12CommandFrame>(device, type, i);

		DXThrowIfFailed(device->CreateCommandList(
			0,
			type,
			commandFrames[0]->GetCommandAllocator(), // Associated command allocator
			nullptr,								 // Initial PipelineStateObject
			IID_PPV_ARGS(commandList.GetAddressOf())));
		DXThrowIfFailed(commandList->Close());
		DXNAME_D3D12_OBJECT(commandList, "DirectX 12 Object name: VWolf DirectX 12 Command List.");

		DXThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		DXNAME_D3D12_OBJECT(fence, "DirectX 12 Object name: VWolf DirectX 12 Fence.");
		fence_event = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	}

	DX12Command::~DX12Command()
	{
	}

	void DX12Command::BeginFrame()
	{

		Ref<DX12CommandFrame> frame = commandFrames[currentFrame];

		frame->Wait(fence_event, fence);
		DXThrowIfFailed(frame->GetCommandAllocator()->Reset());
		DXThrowIfFailed(commandList->Reset(frame->GetCommandAllocator(), nullptr));
	}

	void DX12Command::EndFrame(Ref<DX12Surface> surface)
	{
		DXThrowIfFailed(commandList->Close());
		ID3D12CommandList* cmdsLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		surface->Present();

		++currentFence;
		
		Ref<DX12CommandFrame> frame = commandFrames[currentFrame];
		frame->SetFenceValue(currentFence);

		commandQueue->Signal(fence.Get(), currentFence);
		currentFrame++;
		if (currentFrame == commandFrames.size()) currentFrame = 0;
	}

	void DX12Command::Flush()
	{
		if (!commandFrames.size()) return;
		for (UINT i = 0; i < commandFrames.size(); i++)
			commandFrames[i]->Wait(fence_event, fence);
		currentFrame = 0;
		// NOT SURE HERE. WILL DO IT JUST TO RESIZE THE WINDOW
		/*currentFence = 0;
		Ref<DX12CommandFrame> frame = commandFrames[currentFrame];
		DXThrowIfFailed(frame->GetCommandAllocator()->Reset());
		DXThrowIfFailed(commandList->Reset(frame->GetCommandAllocator(), nullptr));*/
	}

	UINT64 DX12Command::GetCompletedFence() {
		return fence->GetCompletedValue();
	}

	DX12Command::DX12CommandFrame::DX12CommandFrame(Microsoft::WRL::ComPtr<ID3D12Device8> device, D3D12_COMMAND_LIST_TYPE type, UINT index): index(index)
	{
		DXThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));		
		DXNAME_D3D12_OBJECT_INDEXED(commandAllocator, "DirectX 12 Object name: VWolf DirectX 12 Command Allocator", index);
	}

	DX12Command::DX12CommandFrame::~DX12CommandFrame()
	{
	}

	void DX12Command::DX12CommandFrame::Wait(HANDLE fence_event, Microsoft::WRL::ComPtr<ID3D12Fence> fence)
	{
		VWOLF_CORE_ASSERT(fence.Get() && fence_event);
		UINT64 completedValue = fence->GetCompletedValue();
		if (completedValue < value)
		{
			// Fire event when GPU hits current fence.  
			DXThrowIfFailed(fence->SetEventOnCompletion(value, fence_event));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(fence_event, INFINITE);
			//CloseHandle(fence_event);
		}
	}
}

#endif