#pragma once

#include "VWolf/Core/Render/RHI/Driver.h"

#include "OpenGLDevice.h"
#include "OpenGLRenderContext.h"
#include "OpenGLCommandQueue.h"
#include "OpenGLRenderPass.h"

namespace VWolf { 
	class OpenGLDriver : public Driver {
	public:
        virtual ~OpenGLDriver() override {};
		virtual void Initialize(InitConfiguration config) override;
		virtual void Shutdown() override;
		virtual void OnUpdate() override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
        
        Ref<RHI::OpenGLDevice> GetDevice() { return device; }
        Ref<RHI::OpenGLRenderContext> GetRenderContext() { return renderContext; }
        Ref<RHI::OpenGLRenderPattern> GetMainRenderPattern() { return mainRenderPattern; }
        Ref<RHI::OpenGLCommandQueue> GetCommandQueue() { return commandQueue; }
        Ref<RHI::ObjectRegistry> GetObjectRegistry() { return registry; }
		static Ref<OpenGLDriver> GetCurrentDriver() { return currentDriver; }
    private:
        void ResizeIfNeeded();
        
        Ref<RHI::ObjectRegistry> registry;
        Ref<RHI::OpenGLDevice> device;
        Ref<RHI::OpenGLRenderContext> renderContext;
        Ref<RHI::OpenGLRenderPattern> mainRenderPattern;
        Ref<RHI::OpenGLCommandQueue> commandQueue;
        
		static Ref<OpenGLDriver> currentDriver;
        unsigned int m_Width, m_Height;
        bool m_resize = false;
	};
}
