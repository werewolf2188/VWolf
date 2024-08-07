#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Driver.h"

#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/DirectX12/UI/DirectX12UIManager.h"
#include "VWolf/Platform/DirectX12/Windows/WinWindow.h"

#include "VWolf/Core/Render/Graphics.h"
#include "VWolf/Platform/DirectX12/Render/DirectX12Graphics.h"

#include "VWolf/Core/Time.h"

#include "VWolf/Platform/DirectX12/Core/DX12Core.h"
#include "VWolf/Platform/DirectX12/Core/DX12Factory.h"
#include "VWolf/Platform/DirectX12/Core/DX12Device.h"
#include "VWolf/Platform/DirectX12/Core/DX12Command.h"
#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"
#include "VWolf/Platform/DirectX12/Core/DX12Surface.h"

namespace VWolf {

	// TODO: Move
	const UINT numFrames = 2;
	//HMODULE pixModule;

	class WindowsTime : public Time {
	public:
		WindowsTime() {
			QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
			m_offset = Private_QueryTime();
			
		}
	protected:
		virtual float GetTime() override {
			return Private_GetTime();
		};
	private:
		double Private_GetTime() {
			 return (double)(Private_QueryTime() - m_offset) /
				 m_frequency;
		}

		uint64_t Private_QueryTime() {
			uint64_t value;
			QueryPerformanceCounter((LARGE_INTEGER*)&value);
			return value;
		}
	private:
		uint64_t m_frequency = 0;
		uint64_t m_offset = 0;
	};

	DirectX12Driver* DirectX12Driver::currentDriver = nullptr;

	void DirectX12Driver::Initialize(InitConfiguration config, WindowEventCallback& callback)
	{
		handle = GetModuleHandle(nullptr);
		this->callback = &callback;
		window = CreateRef<WinWindow>(handle, config, *this);
		window->Initialize();		
		currentDriver = this;

//#if defined(USE_PIX)
//		pixModule = PIXLoadLatestWinPixGpuCapturerLibrary();
//#endif

		// NEW CLASSES
		if (DX12Factory::Initialize()) {
			device = DX12Factory::CreateDevice();

			commands = device->CreateCommand(numFrames);

			rtvHeap = CreateRef<DX12DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			dsvHeap = CreateRef<DX12DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			srvHeap = CreateRef<DX12DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			// Initialize Descriptor heaps
			rtvHeap->Initialize(512, false);
			dsvHeap->Initialize(512, false);
			srvHeap->Initialize(4096, true);

			// Initialize Surface
			surface = CreateRef<DX12Surface>((HWND__*)window->GetNativeWindow(), config.width, config.height);
			surface->InitializeSwapChain(DX12Factory::GetFactory(), commands->GetCommandQueue(), DXGI_FORMAT_R8G8B8A8_UNORM);

			// Initialize Depth Stencil Buffer
			DX12TextureResourceInfo info;
			info.CreateDepthStencilInformation(device, config.width, config.height);
			depthStencilBuffer = CreateRef<DX12DepthBufferResource>(info);
			depthStencilBuffer->Create(device, dsvHeap);
		}
		else {
			VWOLF_CORE_ASSERT(false, "Factory could not be initialized");
		}

		UIManager::SetDefault(CreateRef<DirectX12UIManager>((HWND__*)window->GetNativeWindow()));
		Time::SetTimeImplementation(CreateRef<WindowsTime>());
		graphics = CreateRef<DirectX12Graphics>();
		Graphics::SetGraphicsImpl(graphics);
		graphics->Initialize();
		initialized = true;
	}

	void DirectX12Driver::OnUpdate() {
		window->OnUpdate();
	}

	void DirectX12Driver::Shutdown()
	{
		DX12Factory::Shutdown();
//#if defined(USE_PIX)
//		if (pixModule)
//			FreeLibrary(pixModule);
//#endif
	}

	void DirectX12Driver::OnEvent(Event& evt) {
		callback->OnEvent(evt);
	}

    void DirectX12Driver::Resize(unsigned int m_Width, unsigned int m_Height) {
        if (m_Width == 0 || m_Height == 0 || !initialized) return;
		commands->Flush();
		graphics->ClearResources(true);
		surface->Resize(m_Width, m_Height);
		depthStencilBuffer->SetSize(m_Width, m_Height);
		depthStencilBuffer->Create(device, dsvHeap);
    }
	UINT DirectX12Driver::GetNumberOfFrames()
	{
		return numFrames;
	}
}
#endif
