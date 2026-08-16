//
//  MetalUIManager.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#include "vwpch.h"

#include <imgui.h>

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalUIManager.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalDriver.h"

#define IMGUI_IMPL_METAL_CPP_EXTENSIONS
#define IMGUI_IMPL_METAL_CPP
#undef __OBJC__

#include "VWolf/Platform/ImGUI/backends/imgui_impl_metal.h"
#include "VWolf/Platform/ImGUI/backends/imgui_impl_osx.h"

#include "VWolf/Platform/Metal/Render/MetalGraphics.h"

namespace VWolf {

    extern NS::View* GetView(Ref<Window> window);

    MetalUIManager::MetalUIManager() {}

    void MetalUIManager::Initialize() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplOSX_Init(GetView(MetalDriver::GetCurrent()->GetWindow()));
        ImGui_ImplMetal_Init(MetalDriver::GetCurrent()->GetDevice()->GetDevice().get());
    }

    void MetalUIManager::Terminate() {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplOSX_Shutdown();
        ImGui::DestroyContext();
    }
    void MetalUIManager::Render() {
        ImGui::Render();
        ImDrawData* data = ImGui::GetDrawData();
        data->FramebufferScale = { 1, 1 };
        ImGui_ImplMetal_RenderDrawData(data,
                                       MetalDriver::GetCurrent()->GetNativeGraphics()->GetCommandList()->GetCommandBuffer().get(),
                                       MetalDriver::GetCurrent()->GetNativeGraphics()->GetCommandList()->GetEncoder().get());
    }

    void MetalUIManager::NewFrame() {
        ImGui_ImplMetal_NewFrame(MetalDriver::GetCurrent()->GetMainRenderPass()->GetRenderPassDescriptor().get());
        ImGui_ImplOSX_NewFrame(GetView(MetalDriver::GetCurrent()->GetWindow()));
        UIManager::NewFrame();
    }
}

#endif
