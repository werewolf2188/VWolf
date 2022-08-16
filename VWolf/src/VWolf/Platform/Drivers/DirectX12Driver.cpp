#include "vwpch.h"

#ifdef VW_PLATFORM_WINDOWS
#include "DirectX12Driver.h"

#include "VWolf/Platform/Windows/WinWindow.h"

// TODO: Remove
#define SwapChainBufferCount 2

// TODO: This could live somewhere else
struct DirectX12Context {
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;
	bool m4xMsaaState = false;    // 4X MSAA enabled
	UINT m4xMsaaQuality = 0;      // quality level of 4X MSAA
	UINT64 mCurrentFence = 0;
	int mCurrBackBuffer = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;
};

namespace VWolf {
	void DirectX12Driver::Initialize(InitConfiguration config)
	{
		context = new DirectX12Context();
		handle = GetModuleHandle(nullptr);
		window = new WinWindow(handle, config);
		window->Initialize();

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


		context->mRtvDescriptorSize = context->md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		context->mDsvDescriptorSize = context->md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		context->mCbvSrvUavDescriptorSize = context->md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

		//D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		//DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = mBackBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(context->md3dDevice->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));
		context->m4xMsaaQuality = msQualityLevels.NumQualityLevels;

		// Command objects

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(context->md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&context->mCommandQueue)));

		ThrowIfFailed(context->md3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(context->mDirectCmdListAlloc.GetAddressOf())));

		ThrowIfFailed(context->md3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			context->mDirectCmdListAlloc.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(context->mCommandList.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		context->mCommandList->Close();

		// Swap chains
		// Release the previous swapchain we will be recreating.
		context->mSwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = config.width;
		sd.BufferDesc.Height = config.height;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = mBackBufferFormat;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = context->m4xMsaaState ? 4 : 1;
		sd.SampleDesc.Quality = context->m4xMsaaState ? (context->m4xMsaaQuality - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SwapChainBufferCount;
		sd.OutputWindow = (HWND)((WinWindow*)window)->GetHWND(); //<----- hwnd
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		ThrowIfFailed(context->mdxgiFactory->CreateSwapChain(
			context->mCommandQueue.Get(),
			&sd,
			context->mSwapChain.GetAddressOf()));

		// RTVs and DSVs descriptor heaps
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(context->md3dDevice->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(context->mRtvHeap.GetAddressOf())));


		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(context->md3dDevice->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(context->mDsvHeap.GetAddressOf())));

		auto flush = [this]() {
			// Advance the fence value to mark commands up to this fence point.
			this->context->mCurrentFence++;

			// Add an instruction to the command queue to set a new fence point.  Because we 
			// are on the GPU timeline, the new fence point won't be set until the GPU finishes
			// processing all the commands prior to this Signal().
			ThrowIfFailed(this->context->mCommandQueue->Signal(this->context->mFence.Get(), this->context->mCurrentFence));

			// Wait until the GPU has completed commands up to this fence point.
			if (this->context->mFence->GetCompletedValue() < this->context->mCurrentFence)
			{
				HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

				// Fire event when GPU hits current fence.  
				ThrowIfFailed(this->context->mFence->SetEventOnCompletion(this->context->mCurrentFence, eventHandle));

				// Wait until the GPU hits current fence event is fired.
				WaitForSingleObject(eventHandle, INFINITE);
				CloseHandle(eventHandle);
			}
		};

		auto resize = [this, flush, config, mBackBufferFormat]() {
			flush();
			ThrowIfFailed(this->context->mCommandList->Reset(this->context->mDirectCmdListAlloc.Get(), nullptr));

			// Release the previous resources we will be recreating.
			for (int i = 0; i < SwapChainBufferCount; ++i)
				this->context->mSwapChainBuffer[i].Reset();
			this->context->mDepthStencilBuffer.Reset();

			// Resize the swap chain.
			ThrowIfFailed(this->context->mSwapChain->ResizeBuffers(
				SwapChainBufferCount,
				config.width, config.height,
				mBackBufferFormat,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

			this->context->mCurrBackBuffer = 0;

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(this->context->mRtvHeap->GetCPUDescriptorHandleForHeapStart());
			for (UINT i = 0; i < SwapChainBufferCount; i++)
			{
				ThrowIfFailed(this->context->mSwapChain->GetBuffer(i, IID_PPV_ARGS(&this->context->mSwapChainBuffer[i])));
				this->context->md3dDevice->CreateRenderTargetView(this->context->mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
				rtvHeapHandle.Offset(1, this->context->mRtvDescriptorSize);
			}

			// Create the depth/stencil buffer and view.
			D3D12_RESOURCE_DESC depthStencilDesc;
			depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			depthStencilDesc.Alignment = 0;
			depthStencilDesc.Width = config.width;
			depthStencilDesc.Height = config.height;
			depthStencilDesc.DepthOrArraySize = 1;
			depthStencilDesc.MipLevels = 1;

			// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
			// the depth buffer.  Therefore, because we need to create two views to the same resource:
			//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
			//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
			// we need to create the depth buffer resource with a typeless format.  
			depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

			depthStencilDesc.SampleDesc.Count = this->context->m4xMsaaState ? 4 : 1;
			depthStencilDesc.SampleDesc.Quality = this->context->m4xMsaaState ? (this->context->m4xMsaaQuality - 1) : 0;
			depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE optClear;
			optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//mDepthStencilFormat;
			optClear.DepthStencil.Depth = 1.0f;
			optClear.DepthStencil.Stencil = 0;
			ThrowIfFailed(this->context->md3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&depthStencilDesc,
				D3D12_RESOURCE_STATE_COMMON,
				&optClear,
				IID_PPV_ARGS(this->context->mDepthStencilBuffer.GetAddressOf())));

			// Create descriptor to mip level 0 of entire resource using the format of the resource.
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//mDepthStencilFormat;
			dsvDesc.Texture2D.MipSlice = 0;
			this->context->md3dDevice->CreateDepthStencilView(this->context->mDepthStencilBuffer.Get(), &dsvDesc, this->context->mDsvHeap->GetCPUDescriptorHandleForHeapStart());

			// Transition the resource from its initial state to be used as a depth buffer.
			this->context->mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->context->mDepthStencilBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

			// Execute the resize commands.
			ThrowIfFailed(this->context->mCommandList->Close());
			ID3D12CommandList* cmdsLists[] = { this->context->mCommandList.Get() };
			this->context->mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

			// Wait until resize is complete.
			flush();

			// Update the viewport transform to cover the client area.
			this->context->mScreenViewport.TopLeftX = 0;
			this->context->mScreenViewport.TopLeftY = 0;
			this->context->mScreenViewport.Width = static_cast<float>(config.width);
			this->context->mScreenViewport.Height = static_cast<float>(config.height);
			this->context->mScreenViewport.MinDepth = 0.0f;
			this->context->mScreenViewport.MaxDepth = 1.0f;

			this->context->mScissorRect = { 0, 0, config.width, config.height };
		};

		resize();

		((WinWindow*)window)->clearFunc = [this, flush]() {

			
			// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
			ThrowIfFailed(this->context->mDirectCmdListAlloc->Reset());

			// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
			// Reusing the command list reuses memory.
			ThrowIfFailed(this->context->mCommandList->Reset(this->context->mDirectCmdListAlloc.Get(), nullptr));

			// Indicate a state transition on the resource usage.
			this->context->mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->context->mSwapChainBuffer[this->context->mCurrBackBuffer].Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
			this->context->mCommandList->RSSetViewports(1, &this->context->mScreenViewport);
			this->context->mCommandList->RSSetScissorRects(1, &this->context->mScissorRect);

			// Clear the back buffer and depth buffer.
			this->context->mCommandList->ClearRenderTargetView(CD3DX12_CPU_DESCRIPTOR_HANDLE(
				this->context->mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
				this->context->mCurrBackBuffer,
				this->context->mRtvDescriptorSize), DirectX::Colors::LightSteelBlue, 0, nullptr);
			this->context->mCommandList->ClearDepthStencilView(this->context->mDsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

			// Specify the buffers we are going to render to.
			this->context->mCommandList->OMSetRenderTargets(1, &CD3DX12_CPU_DESCRIPTOR_HANDLE(
				this->context->mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
				this->context->mCurrBackBuffer,
				this->context->mRtvDescriptorSize), true, &this->context->mDsvHeap->GetCPUDescriptorHandleForHeapStart());

			// Indicate a state transition on the resource usage.
			this->context->mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->context->mSwapChainBuffer[this->context->mCurrBackBuffer].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
			
			

			// Done recording commands.
			ThrowIfFailed(this->context->mCommandList->Close());

			// Add the command list to the queue for execution.
			ID3D12CommandList* cmdsLists[] = { this->context->mCommandList.Get() };
			this->context->mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

			// swap the back and front buffers
			ThrowIfFailed(this->context->mSwapChain->Present(0, 0));
			this->context->mCurrBackBuffer = (this->context->mCurrBackBuffer + 1) % 2;

			// Wait until frame commands are complete.  This waiting is inefficient and is
			// done for simplicity.  Later we will show how to organize our rendering code
			// so we do not have to wait per frame.
			flush();
		};
	}

	void DirectX12Driver::Shutdown()
	{
		delete context;
		delete window;
	}
}
#endif