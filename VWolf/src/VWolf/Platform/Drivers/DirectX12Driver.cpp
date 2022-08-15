#include "vwpch.h"

#ifdef VW_PLATFORM_WINDOWS

// Direct 3D stuff
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
//#include <D3Dcompiler.h>
//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
//#include <DirectXColors.h>
//#include <DirectXCollision.h>

// This will go somewhere later
#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
    HRESULT hr__ = (x); \
    if(FAILED(hr__)) { std::cout << "DirectX 12 Error: File" << __FILE__ << ". Line: " << __LINE__ << std::endl; return; } \
}
#endif
// 

#include "DirectX12Driver.h"

#include "VWolf/Platform/Windows/WinWindow.h"

// TODO: This could live somewhere else
struct DirectX12Context {
	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;
	bool m4xMsaaState = false;    // 4X MSAA enabled
	UINT m4xMsaaQuality = 0;      // quality level of 4X MSAA

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
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
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
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
		sd.BufferCount = 2;
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
		rtvHeapDesc.NumDescriptors = 2;
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

		window->Run();
	}

	void DirectX12Driver::Shutdown()
	{
		delete context;
		delete window;
	}
}
#endif