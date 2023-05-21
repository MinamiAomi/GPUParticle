// このヘッダーでインクルードされたヘッダーファイルは
// インクルードしなくていい

#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>

#include <array>
#include <format>
#include <functional>
#include <list>
#include <iomanip>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <Windows.h>
#include <wrl.h>

// Externals
#include <d3dx12.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")