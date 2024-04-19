//
//  Bridge.m
//  VWolf
//
//  Created by Enrique Ricalde on 4/21/24.
//

/// This is a bridge to connect objc with c++
/// .mm files can connect with .cpp

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "Core.h"

NS::View* ConnectLayer(NS::Window* window, CA::MetalLayer* layer) {
    CAMetalLayer* _layer = (__bridge CAMetalLayer*)(layer);
    NSWindow* _window = (__bridge NSWindow*)(window);

    NSView* view = _window.contentView;
    view.layer = _layer;
    view.wantsLayer = YES;
    return (__bridge NS::View*)view;
}

float GetFactorScale(NS::View* view) {
    NSView* _view = (__bridge NSView*)(view);
    return _view.window.screen.backingScaleFactor ?: NSScreen.mainScreen.backingScaleFactor;
}

#endif
