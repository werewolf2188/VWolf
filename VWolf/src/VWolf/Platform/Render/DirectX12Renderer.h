#pragma once

#include "VWolf/Core/Render/Renderer.h"

#include "HLSLShader.h"
#include "DirectX12Buffer.h"
#include "DirectX12BufferGroup.h"
#include "DirectX12RenderAPI.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {
	class DirectX12Renderer : public Renderer {
    public:
		DirectX12Renderer(HWND__* window,
			DirectX12Context* context);
        virtual ~DirectX12Renderer() override {};
    protected:
        virtual void ProcessItems() override;
    private:
        HWND__* window;
        DirectX12Context* context;
        int frame = 0;
        std::vector<Ref<BufferGroup>> groups;
	};
}