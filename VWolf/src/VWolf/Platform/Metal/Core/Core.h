//
//  Core.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once
#include <MetalKit/MetalKit.hpp>
#include <AppKit/AppKit.hpp>
#include <Metal/Metal.hpp>

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)

#define NSSTRINGFROM(charArray) NS::String::string(charArray, NS::StringEncoding::UTF8StringEncoding)

NS::View* ConnectLayer(NS::Window* window, CA::MetalLayer* layer);
float GetFactorScale(NS::View* view);

static void GLFWErrorCallback(int error, const char* description)
{
    VWOLF_CORE_ERROR("GLFW Error (%d): %s", error, description);
}
#endif
