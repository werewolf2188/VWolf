#pragma once

#define VWOLF_CORE

#include "VWolf/Core/PlatformDetection.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <mutex>
#include <stdexcept>

#include <string>
#include <codecvt>
#include <locale>
#include <sstream>
#include <array>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>

// BOOST
#include <boost/core/null_deleter.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

// BOOST UUID
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

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

// OPENGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef VWOLF_PLATFORM_WINDOWS
#define NOMINMAX 1
#include <Windows.h>
#include <windowsx.h>
#include <wrl.h>
#include <dxgi1_4.h> 
#include <dxgi1_6.h> // This will replace above
#include <d3d12.h>
//#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dx12.h>
#include <D3Dcompiler.h>
#include <d3d11shader.h>
#include <DirectXPackedVector.h>
//#if defined(_DEBUG) || defined(DEBUG)
//#define USE_PIX
//#include "pix3.h"
//#endif
//#include <DirectXCollision.h>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#endif

#include "VWolf/Core/Log.h"
