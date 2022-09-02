#include "vwpch.h"
#include "DirectX12UIManager.h"

#include <imgui.h>

#ifdef VWOLF_PLATFORM_WINDOWS

#include "VWolf/Platform/UI/backends/imgui_impl_win32.h"
#include "VWolf/Platform/UI/backends/imgui_impl_dx12.h"

#define NUM_FRAMES_IN_FLIGHT 2

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace VWolf {
	DirectX12UIManager::DirectX12UIManager(HWND__* window, DirectX12Context* context) : m_window(window), context(context)
	{
	}
	void DirectX12UIManager::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(m_window);
		ImGui_ImplDX12_Init(context->md3dDevice.Get(), NUM_FRAMES_IN_FLIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM, context->mSrvHeap.Get(),
			context->mSrvHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
			context->mSrvHeap.Get()->GetGPUDescriptorHandleForHeapStart());
	}
	void DirectX12UIManager::Terminate()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	void DirectX12UIManager::Render()
	{	
		ImGui::Render();
		dx12ResetCommandList(context);

		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// Clear the back buffer and depth buffer.
		dx12ClearRenderTargetView(context, DirectX::Colors::Green);
		dx12ClearDepthStencilView(context);
		
		// Specify the buffers we are going to render to.
		dx12SetRenderTarget(context);

		context->mCommandList->SetDescriptorHeaps(1, context->mSrvHeap.GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context->mCommandList.Get());

		// Indicate a state transition on the resource usage.
		dx12ResourceBarrierTransitionForCurrentBackBuffer(context, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		dx12ExecuteCommands(context);		
	}
	void DirectX12UIManager::NewFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
	}

	LRESULT DirectX12UIManager::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return ImGui_ImplWin32_WndProcHandler(m_window, uMsg, wParam, lParam);
	}
}
#endif