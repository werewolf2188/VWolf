#pragma once

#define VWOLF_CORE

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

// BOOST
#include <boost/core/null_deleter.hpp>
#include <boost/lexical_cast.hpp>

// BOOST LOG
#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#ifdef VWOLF_PLATFORM_WINDOWS
#include <Windows.h>
#include <windowsx.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dx12.h>
#include <d3dx12Context.h>
#include <d3dx12Commands.h>
//#include <D3Dcompiler.h>
//#include <DirectXPackedVector.h>
//#include <DirectXCollision.h>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#endif

#include "VWolf/Core/Log.h"