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
// TODO: Move DirectX libraries 
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#endif