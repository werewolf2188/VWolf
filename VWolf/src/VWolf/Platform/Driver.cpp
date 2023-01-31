#include "vwpch.h"

#include "VWolf/Core/Driver.h"

#include "VWolf/Platform/OpenGL/OpenGLDriver.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/DirectX12/DirectX12Driver.h"
#endif

namespace VWolf {
	class NullDriver : public Driver {
    public:
        virtual ~NullDriver() override {};
		virtual void Initialize(InitConfiguration config, WindowEventCallback& callback) override {};
		virtual void Shutdown() override {};
		virtual void OnUpdate() override {};
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override {};
	};

	Scope<Driver> Driver::GetDriver(DriverType type) {
		switch (type) {
#ifdef VWOLF_PLATFORM_WINDOWS
		case DriverType::DirectX12: return CreateScope<DirectX12Driver>();
#endif
		case DriverType::OpenGL: return  CreateScope<OpenGLDriver>();

		default: break;
		}		
		VWOLF_CORE_ASSERT(false, "The driver is not yet fully implemented");
		return CreateScope<NullDriver>();
	}
}
