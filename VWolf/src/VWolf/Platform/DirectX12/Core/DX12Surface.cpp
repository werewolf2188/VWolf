#include "vwpch.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "DX12Surface.h"
#include "DX12Core.h"

#include "DX12Device.h"
#include "DX12Resources.h"

#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

namespace VWolf {
	DX12Surface::~DX12Surface()
	{
	}
	void DX12Surface::InitializeSwapChain(Microsoft::WRL::ComPtr<IDXGIFactory7> factory, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, DXGI_FORMAT format)
	{
		// TODO: Remove reference of DirectX12Driver::GetCurrent()->GetNumberOfFrames()
		DXGI_SWAP_CHAIN_DESC1 sd;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Width = width;
		sd.Height = height;
		sd.Format = format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB? DXGI_FORMAT_R8G8B8A8_UNORM: format;
		sd.Scaling = DXGI_SCALING_STRETCH; // This could be changed later on
		sd.SampleDesc.Count = 1; //;DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() ? 4 : 1;
		sd.SampleDesc.Quality = 0; // = DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() ? (DirectX12Driver::GetCurrent()->GetDevice()->GetMSAAQuality() - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = DirectX12Driver::GetCurrent()->GetNumberOfFrames();
		sd.Stereo = false;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // In case we use the back buffer as an image for later
		sd.Flags = 0;

		IDXGISwapChain1* swapChain1;
		DXThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(), window, &sd, nullptr, nullptr, &swapChain1));
		DXThrowIfFailed(swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain)));
		swapChain1->Release();

		currentBackBuffer = swapChain->GetCurrentBackBufferIndex();
		this->format = format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM : format;
		renderTargetViews = std::vector<Ref<DX12TextureResource>>(DirectX12Driver::GetCurrent()->GetNumberOfFrames());
		Finalize();
	}
	void DX12Surface::Present()
	{
		VWOLF_CORE_ASSERT(swapChain);
		DXThrowIfFailed(swapChain->Present(0, 0));
		currentBackBuffer = swapChain->GetCurrentBackBufferIndex();
	}
	void DX12Surface::Finalize() {

		for (int i = 0; i < DirectX12Driver::GetCurrent()->GetNumberOfFrames(); i++) {
			if (!renderTargetViews[i].get())
				renderTargetViews[i] = CreateRef<DX12TextureResource>(format);
			renderTargetViews[i]->CreateSurfaceRenderTargetResource(i, this, DirectX12Driver::GetCurrent()->GetDevice(), DirectX12Driver::GetCurrent()->GetRenderTargetViewDescriptorHeap());
		}

		screenViewport.TopLeftX = 0;
		screenViewport.TopLeftY = 0;
		screenViewport.Width = static_cast<float>(width);
		screenViewport.Height = static_cast<float>(height);
		screenViewport.MinDepth = 0.0f;
		screenViewport.MaxDepth = 1.0f;

		// Does it have to be long?
		scissorRect = { 0, 0, static_cast<long>(width), static_cast<long>(height) };
	}
	Ref<DX12TextureResource> DX12Surface::GetCurrentRenderTargetView()
	{
		return renderTargetViews[currentBackBuffer];
	}

	void DX12Surface::Resize(UINT width, UINT height)
	{
		this->width = width;
		this->height = height;
		for (int i = 0; i < DirectX12Driver::GetCurrent()->GetNumberOfFrames(); i++) {
			renderTargetViews[i]->GetResource().Reset();
		}
		/*renderTargetViews.clear();
		renderTargetViews = std::vector<Ref<DX12TextureResource>>(DirectX12Driver::GetCurrent()->GetNumberOfFrames());*/
		currentBackBuffer = 0;

		DXThrowIfFailed(swapChain->ResizeBuffers(
			DirectX12Driver::GetCurrent()->GetNumberOfFrames(),
			width, height,
			format,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
		Finalize();
	}
}

#endif