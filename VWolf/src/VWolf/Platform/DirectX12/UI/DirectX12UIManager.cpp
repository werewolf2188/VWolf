#include "vwpch.h"


#include <imgui.h>

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12UIManager.h"
#include "VWolf/Platform/DirectX12/DirectX12Driver.h"

#include "VWolf/Platform/DirectX12/Core/DX12Device.h"
#include "VWolf/Platform/DirectX12/Core/DX12Command.h"

#include "VWolf/Platform/ImGUI/backends/imgui_impl_win32.h"
#include "VWolf/Platform/ImGUI/backends/imgui_impl_dx12.h"

#define NUM_FRAMES_IN_FLIGHT 2

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace VWolf {

	DirectX12UIManager::DirectX12UIManager(HWND__* window) : m_window(window)
	{
		// TODO: Replace with heap inside driver instead
		/*heap = CreateRef<DX12DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		heap->Initialize(1, true);		*/
	}
	DirectX12UIManager::~DirectX12UIManager()
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
		auto descriptor = DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap()->Allocate();
		ImGui_ImplDX12_Init(DirectX12Driver::GetCurrent()->GetDevice()->GetDevice().Get(), NUM_FRAMES_IN_FLIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM, DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap()->GetHeap().Get(),
			descriptor.GetCPUAddress(),
			descriptor.GetGPUAddress());
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

		DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList()
			->SetDescriptorHeaps(1, DirectX12Driver::GetCurrent()->GetShaderResourceViewDescriptorHeap()->GetHeap().GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectX12Driver::GetCurrent()->GetCommands()->GetCommandList().Get());
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