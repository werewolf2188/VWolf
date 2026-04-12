//
//  Core.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include <MetalKit/MetalKit.hpp>
#include <AppKit/AppKit.hpp>
#include <Metal/Metal.hpp>
#include <metal_irconverter/metal_irconverter.h>

#define NSSTRINGFROM(charArray) NS::String::string(charArray, NS::StringEncoding::UTF8StringEncoding)

NS::View* ConnectLayer(NS::Window* window, CA::MetalLayer* layer);
float GetFactorScale(NS::View* view);

static void MetalErrorCallback(int error, const char* description)
{
    VWOLF_CORE_ERROR("GLFW Error (%d): %s", error, description);
}
#endif
