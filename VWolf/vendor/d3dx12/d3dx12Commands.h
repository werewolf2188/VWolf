#ifndef __D3DX12_COMMANDS_H__
#define __D3DX12_COMMANDS_H__

#include <Windows.h>
#include <wrl.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include "d3dx12Context.h"

#if defined( __cplusplus )

inline void dx12CreateRenderTargetView(DirectX12Context* context) {
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(context->mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(context->mSwapChain->GetBuffer(i, IID_PPV_ARGS(&context->mSwapChainBuffer[i])));
		context->md3dDevice->CreateRenderTargetView(context->mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, context->mRtvDescriptorSize);
	}
}

inline void dx12ResourceBarrierTransition(DirectX12Context* context, ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) {
	context->mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource,
		from, to));
}

// Transition the resource from its initial state to be used as a depth buffer.
inline void dx12ResourceBarrierTransitionForDepthStencilBuffer(DirectX12Context* context, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) {
	dx12ResourceBarrierTransition(context, context->mDepthStencilBuffer.Get(), from, to);
}

inline void dx12ResourceBarrierTransitionForCurrentBackBuffer(DirectX12Context* context, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) {
	dx12ResourceBarrierTransition(context, dx12GetCurrentBackBuffer(context), from, to);
}

inline D3D12_CPU_DESCRIPTOR_HANDLE dx12GetCurrentBackBufferView(DirectX12Context* context) {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		context->mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		context->mCurrBackBuffer,
		context->mRtvDescriptorSize);
}

inline void dx12ClearRenderTargetView(DirectX12Context* context, DirectX::XMVECTORF32 color) {
	context->mCommandList->ClearRenderTargetView(dx12GetCurrentBackBufferView(context), color, 0, nullptr);
}

inline void dx12ClearDepthStencilView(DirectX12Context* context) {
	context->mCommandList->ClearDepthStencilView(context->mDsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

inline void dx12SetRenderTarget(DirectX12Context* context) {
	context->mCommandList->OMSetRenderTargets(1, &dx12GetCurrentBackBufferView(context), true, &context->mDsvHeap->GetCPUDescriptorHandleForHeapStart());
}

inline void dx12ResizeBuffers(DirectX12Context* context, UINT width, UINT height) {
	dx12Flush(context);
	dx12ResetCommandList(context);
	dx12ResizeSwapChainBuffers(context, width, height);

	dx12CreateRenderTargetView(context);

	dx12CreateDepthStencilBuffer(context, width, height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT));
	dx12CreateDepthStencilView(context);

	dx12ResourceBarrierTransitionForDepthStencilBuffer(context, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	dx12ExecuteCommands(context);
	// Wait until resize is complete.
	dx12Flush(context);
	dx12UpdateClientArea(context, width, height);
}

#endif // #if defined( __cplusplus )
#endif // #define __D3DX12_COMMANDS_H__