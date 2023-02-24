#pragma once

#include "VWolf/Core/Driver.h"

struct DirectX12Context;

struct HINSTANCE__;

namespace VWolf {

	class DX12Device;
	class DX12Command;
	class DX12DescriptorHeap;
	class DX12Surface;
	class DX12DepthBufferResource;

	class DirectX12Graphics;

	class DirectX12Driver : public Driver, public WindowEventCallback {
	public:
        virtual ~DirectX12Driver() override {};
		virtual void Initialize(InitConfiguration config, WindowEventCallback& callback) override;
		virtual void Shutdown() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& evt) override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
	public:
		static DirectX12Driver* GetCurrent() { return currentDriver; }
		Ref<DX12Device> GetDevice() { return device; }
		Ref<DX12Command> GetCommands() { return commands; }
		Ref<DX12Surface> GetSurface() { return surface; }

		Ref<DX12DescriptorHeap> GetRenderTargetViewDescriptorHeap() { return rtvHeap; }
		Ref<DX12DescriptorHeap> GetDepthStencilViewDescriptorHeap() { return dsvHeap; }
		Ref<DX12DescriptorHeap> GetShaderResourceViewDescriptorHeap() { return srvHeap; }

		Ref<DX12DepthBufferResource> GetDepthStencilBuffer() { return depthStencilBuffer; }

		UINT GetNumberOfFrames();
	private:
		HINSTANCE__* handle;
		WindowEventCallback* callback;
		Ref<DX12Device> device;
		Ref<DX12Command> commands;
		Ref<DirectX12Graphics> graphics;

		Ref<DX12DescriptorHeap> rtvHeap;
		Ref<DX12DescriptorHeap> dsvHeap;
		Ref<DX12DescriptorHeap> srvHeap;
		Ref<DX12Surface> surface; // Includes swap chain, viewport and scissors rect;

		Ref<DX12DepthBufferResource> depthStencilBuffer;

		bool initialized = false;
	private:
		static DirectX12Driver* currentDriver;
	};
}