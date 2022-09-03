#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Driver.h"

#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/UI/DirectX12UIManager.h"
#include "VWolf/Platform/Windows/WinWindow.h"

namespace VWolf {
	void DirectX12Driver::Initialize(InitConfiguration config, WindowEventCallback& callback)
	{
		handle = GetModuleHandle(nullptr);
		this->callback = &callback;
		window = CreateRef<WinWindow>(handle, config, *this);
		window->Initialize();		

		dx12InitializeDefaultContext(context, config.width, config.height, ((WinWindow*)window.get())->GetHWND())

		// Same for resize
		dx12ResizeBuffers(context, config.width, config.height);

		UIManager::SetDefault(CreateRef<DirectX12UIManager>(((WinWindow*)window.get())->GetHWND(), context));

		((WinWindow*)window.get())->clearFunc = [this]() {
			
			dx12ResetCommandListAllocator(this->context);
			dx12ResetCommandList(this->context);

			dx12SetCommandListClientArea(this->context);

			// Indicate a state transition on the resource usage.
			dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			// Clear the back buffer and depth buffer.
			dx12ClearRenderTargetView(this->context, DirectX::Colors::Green);
			dx12ClearDepthStencilView(this->context);

			// Specify the buffers we are going to render to.
			dx12SetRenderTarget(this->context);

			// Indicate a state transition on the resource usage.
			dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			
			dx12ExecuteCommands(this->context);			
		};
	}

	void DirectX12Driver::OnUpdate() {
		window->OnUpdate();
		dx12SwapBuffers(this->context);
		// Wait until frame commands are complete.  This waiting is inefficient and is
		// done for simplicity.  Later we will show how to organize our rendering code
		// so we do not have to wait per frame.
		dx12Flush(this->context);
	}

	void DirectX12Driver::Shutdown()
	{
		delete context;
	}

	void DirectX12Driver::OnEvent(Event& evt) {
		callback->OnEvent(evt);
	}
}
#endif