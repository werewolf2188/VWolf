#include "vwpch.h"
#include "DirectX12UIManager.h"

#include <imgui.h>

#ifdef VWOLF_PLATFORM_WINDOWS

#include "VWolf/Platform/UI/backends/imgui_impl_win32.h"
#include "VWolf/Platform/UI/backends/imgui_impl_dx12.h"

#define NUM_FRAMES_IN_FLIGHT 2

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace VWolf {
	struct UIContext {
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
	};

	DirectX12UIManager::DirectX12UIManager(HWND__* window, DirectX12Context* context) : m_window(window), context(context)
	{
		m_uiContext = new UIContext();
		dx12InitializeDescriptorHeap(context->md3dDevice, m_uiContext->mSrvHeap, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}
	DirectX12UIManager::~DirectX12UIManager()
	{
		delete m_uiContext;
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
			DXGI_FORMAT_R8G8B8A8_UNORM, m_uiContext->mSrvHeap.Get(),
			m_uiContext->mSrvHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
			m_uiContext->mSrvHeap.Get()->GetGPUDescriptorHandleForHeapStart());
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
		
		// Specify the buffers we are going to render to.
		dx12SetRenderTarget(context);

		dx12SetDescriptorHeaps(context, m_uiContext->mSrvHeap);
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
	bool DirectX12UIManager::OnEvent(Event& evt) {
		return false;
	}

	LRESULT DirectX12UIManager::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return ImGui_ImplWin32_WndProcHandler(m_window, uMsg, wParam, lParam);
	}
}
#endif