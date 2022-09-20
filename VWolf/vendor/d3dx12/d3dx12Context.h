#ifndef __D3DX12_CONTEXT_H__
#define __D3DX12_CONTEXT_H__

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>

#if defined( __cplusplus )

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
    HRESULT hr__ = (x); \
    if(FAILED(hr__)) { std::cout << "DirectX 12 Error: File" << __FILE__ << ". Line: " << __LINE__ << std::endl; return; } \
}

#define ThrowIfFailedWithReturnValue(x, y) \
{ \
    HRESULT hr__ = (x); \
    if(FAILED(hr__)) { std::cout << "DirectX 12 Error: File" << __FILE__ << ". Line: " << __LINE__ << std::endl; return y; } \
}
#endif

#define SwapChainBufferCount 2

struct DirectX12Context {
	// For debug purposes
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

	// For device purposes
	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;
	UINT m4xMsaaQuality = 0;      // quality level of 4X MSAA
	UINT64 mCurrentFence = 0;
	int mCurrBackBuffer = 0;
	bool m4xMsaaState = false;    // 4X MSAA enabled		

	// For command queue purposes
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;		
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
};

inline void dx12InitializingDeviceInfo(DirectX12Context* context) {
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer. ????
	{
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&context->debugController)));
		context->debugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&context->mdxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&context->md3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(context->mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&context->md3dDevice)));
	}

	ThrowIfFailed(context->md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&context->mFence)));
}

inline void dx12InitializeDescriptorSizes(DirectX12Context* context) {
	context->mRtvDescriptorSize = context->md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	context->mDsvDescriptorSize = context->md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	context->mCbvSrvUavDescriptorSize = context->md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

inline void dx12SettingMSAA(DirectX12Context* context, 
	UINT sampleCount,
	D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) {
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = context->mBackBufferFormat;
	msQualityLevels.SampleCount = sampleCount;
	msQualityLevels.Flags = flags;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(context->md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));
	context->m4xMsaaQuality = msQualityLevels.NumQualityLevels;
}

inline void dx12InitializeCommandObjects(DirectX12Context* context, 
	D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT, 
	D3D12_COMMAND_QUEUE_FLAGS flags = D3D12_COMMAND_QUEUE_FLAG_NONE) {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = type;
	queueDesc.Flags = flags;
	ThrowIfFailed(context->md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&context->mCommandQueue)));

	ThrowIfFailed(context->md3dDevice->CreateCommandAllocator(
		type,
		IID_PPV_ARGS(context->mDirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(context->md3dDevice->CreateCommandList(
		0,
		type,
		context->mDirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(context->mCommandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	context->mCommandList->Close();
}

inline void dx12CreateSwapChain(DirectX12Context* context,
	UINT width,
	UINT height,
	HWND window,
	UINT swapChainBufferCount = 2,
	DXGI_MODE_SCANLINE_ORDER scanlineOrderMode = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
	DXGI_MODE_SCALING scalingMode = DXGI_MODE_SCALING_UNSPECIFIED,
	DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
	DXGI_SWAP_CHAIN_FLAG flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) {
	// Swap chains
		// Release the previous swapchain we will be recreating.
	context->mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = context->mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = scanlineOrderMode;
	sd.BufferDesc.Scaling = scalingMode;
	sd.SampleDesc.Count = context->m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = context->m4xMsaaState ? (context->m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = swapChainBufferCount;
	sd.OutputWindow = window; //<----- hwnd
	sd.Windowed = true;
	sd.SwapEffect = swapEffect;
	sd.Flags = flags;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(context->mdxgiFactory->CreateSwapChain(
		context->mCommandQueue.Get(),
		&sd,
		context->mSwapChain.GetAddressOf()));
}

// For RTVs and DSVs descriptor heaps
inline void dx12InitializeDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& mHeap,
	UINT numDescriptors,
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE) {
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	heapDesc.NumDescriptors = numDescriptors;
	heapDesc.Type = type;
	heapDesc.Flags = flags;
	heapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(mHeap.GetAddressOf())));
}

#define dx12InitializeDefaultContext(context, width, height, hwnd) \
context = new DirectX12Context(); \
dx12InitializingDeviceInfo(context); \
dx12InitializeDescriptorSizes(context); \
dx12SettingMSAA(context, 4); \
dx12InitializeCommandObjects(context); \
dx12CreateSwapChain(context, width, height, hwnd, SwapChainBufferCount); \
dx12InitializeDescriptorHeap(context->md3dDevice, context->mRtvHeap, SwapChainBufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV); \
dx12InitializeDescriptorHeap(context->md3dDevice, context->mDsvHeap, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);\

inline void dx12Flush(DirectX12Context* context) {
	// Advance the fence value to mark commands up to this fence point.
	context->mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(context->mCommandQueue->Signal(context->mFence.Get(), context->mCurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (context->mFence->GetCompletedValue() < context->mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(context->mFence->SetEventOnCompletion(context->mCurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

inline void dx12ExecuteCommands(DirectX12Context* context) {
	// Execute the resize commands.
	ThrowIfFailed(context->mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { context->mCommandList.Get() };
	context->mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

inline void dx12SwapBuffers(DirectX12Context* context) {
	// swap the back and front buffers
	ThrowIfFailed(context->mSwapChain->Present(0, 0));
	context->mCurrBackBuffer = (context->mCurrBackBuffer + 1) % 2;
}

inline void dx12CreateDepthStencilBuffer(DirectX12Context* context, UINT width, UINT height, D3D12_HEAP_PROPERTIES properties) {
	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = context->m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = context->m4xMsaaState ? (context->m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = context->mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(context->md3dDevice->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(context->mDepthStencilBuffer.GetAddressOf())));
}

inline D3D12_CPU_DESCRIPTOR_HANDLE dx12GetDepthStencilView(DirectX12Context* context) {
	return context->mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

inline void dx12CreateDepthStencilView(DirectX12Context* context) {
	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = context->mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	context->md3dDevice->CreateDepthStencilView(context->mDepthStencilBuffer.Get(), &dsvDesc, dx12GetDepthStencilView(context));
}

inline void dx12ResetCommandList(DirectX12Context* context, Microsoft::WRL::ComPtr<ID3D12PipelineState> pso = nullptr) {
	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
			// Reusing the command list reuses memory.
	ThrowIfFailed(context->mCommandList->Reset(context->mDirectCmdListAlloc.Get(), (bool)pso ? pso.Get(): nullptr));
}

inline void dx12ResetCommandListAllocator(DirectX12Context* context) {			
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(context->mDirectCmdListAlloc->Reset());
}

inline void dx12ResetSwapChainBuffers(DirectX12Context* context, int swapChains) {
	// Release the previous resources we will be recreating.
	for (int i = 0; i < swapChains; ++i)
		context->mSwapChainBuffer[i].Reset();
	context->mDepthStencilBuffer.Reset();
	context->mCurrBackBuffer = 0;
}

inline void dx12ResizeSwapChainBuffers(DirectX12Context* context, UINT width, UINT height) {
	dx12ResetSwapChainBuffers(context, SwapChainBufferCount);
	// Resize the swap chain.
	ThrowIfFailed(context->mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		width, height,
		context->mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
}

inline void dx12UpdateClientArea(DirectX12Context* context, UINT width, UINT height) {
	// Update the viewport transform to cover the client area.
	context->mScreenViewport.TopLeftX = 0;
	context->mScreenViewport.TopLeftY = 0;
	context->mScreenViewport.Width = static_cast<float>(width);
	context->mScreenViewport.Height = static_cast<float>(height);
	context->mScreenViewport.MinDepth = 0.0f;
	context->mScreenViewport.MaxDepth = 1.0f;

	// Does it have to be long?
	context->mScissorRect = { 0, 0, static_cast<long>(width), static_cast<long>(height) };
}

inline void dx12SetCommandListClientArea(DirectX12Context* context) {
	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	context->mCommandList->RSSetViewports(1, &context->mScreenViewport);
	context->mCommandList->RSSetScissorRects(1, &context->mScissorRect);
}

inline ID3D12Resource* dx12GetCurrentBackBuffer(DirectX12Context* context) {
	return context->mSwapChainBuffer[context->mCurrBackBuffer].Get();
}

inline void dx12SetDescriptorHeaps(DirectX12Context* context, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dHeap) {
	context->mCommandList->SetDescriptorHeaps(1, dHeap.GetAddressOf());
}

#endif // #if defined( __cplusplus )
#endif // #define __D3DX12_CONTEXT_H__
