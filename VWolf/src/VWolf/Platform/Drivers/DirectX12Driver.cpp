#include "vwpch.h"

#ifdef VWOLF_PLATFORM_WINDOWS
#include "DirectX12Driver.h"

#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/UI/DirectX12UIManager.h"
#include "VWolf/Platform/Windows/WinWindow.h"

#include "VWolf/Platform/Render/DirectX12RenderAPI.h"
#include "VWolf/Core/Render/Renderer.h"

#include "VWolf/Core/Time.h"

namespace VWolf {

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

	void DirectX12Driver::Initialize(InitConfiguration config, WindowEventCallback& callback)
	{
		handle = GetModuleHandle(nullptr);
		this->callback = &callback;
		window = CreateRef<WinWindow>(handle, config, *this);
		window->Initialize();		

		dx12InitializeDefaultContext(context, config.width, config.height, (HWND__*)window->GetNativeWindow())

		// Same for resize
		dx12ResizeBuffers(context, config.width, config.height);

		UIManager::SetDefault(CreateRef<DirectX12UIManager>((HWND__*)window->GetNativeWindow(), context));
		Renderer::SetRenderAPI(CreateScope<DirectX12RenderAPI>((HWND__*)window->GetNativeWindow(), context));
		Time::SetTimeImplementation(CreateRef<WindowsTime>());
	}

	void DirectX12Driver::OnUpdate() {
		window->OnUpdate();
		dx12SwapBuffers(context);
		// Wait until frame commands are complete.  This waiting is inefficient and is
		// done for simplicity.  Later we will show how to organize our rendering code
		// so we do not have to wait per frame.
		dx12Flush(context);
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