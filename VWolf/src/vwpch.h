#pragma once

#include "VWolf/Core/PlatformDetection.h"

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifdef VW_PLATFORM_WINDOWS
#include <Windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dx12.h>
//#include <D3Dcompiler.h>
//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
#include <DirectXColors.h>
//#include <DirectXCollision.h>

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
    HRESULT hr__ = (x); \
    if(FAILED(hr__)) { std::cout << "DirectX 12 Error: File" << __FILE__ << ". Line: " << __LINE__ << std::endl; return; } \
}
#endif

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#endif