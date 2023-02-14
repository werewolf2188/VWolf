#pragma once
#include "VWolf/Core/Base.h"

class HWND__;

namespace VWolf {
	class DX12TextureResource;

	class DX12Surface {
	public:
		DX12Surface(HWND__* lowLevelWindow, UINT width, UINT height): window(lowLevelWindow), width(width), height(height) {}
		~DX12Surface();
	public:
		void InitializeSwapChain(Microsoft::WRL::ComPtr<IDXGIFactory7> factory, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, DXGI_FORMAT format);
		void Present();
		void Resize(UINT width, UINT height);
		void Finalize();

		Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain() { return swapChain; }
		D3D12_VIEWPORT GetScreenViewport() { return screenViewport; }
		D3D12_RECT GetScissorRect() { return scissorRect; }
		DXGI_FORMAT GetFormat() { return format; }

		Ref<DX12TextureResource> GetCurrentRenderTargetView();
		UINT GetCurrentBackBuffer() { return currentBackBuffer; }
	private:
		HWND__* window; // Should I keep the reference though?
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
		D3D12_VIEWPORT screenViewport;
		D3D12_RECT scissorRect;
		DXGI_FORMAT format;
		
		std::vector<Ref<DX12TextureResource>> renderTargetViews;

		UINT width;
		UINT height;
		UINT currentBackBuffer = 0;
	};
}