// このヘッダーでインクルードされたヘッダーファイルは
// インクルードしなくていい

#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>

#include <array>
#include <format>
#include <functional>
#include <fstream>
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
#include <dinput.h>
#include <Windows.h>
#undef min
#undef max
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
#pragma comment(lib,"dinput8.lib")

//#define DIRECTINPUT_VERSION 0x0800 // DirectIputのバージョン指定

#define DELETE_COPY_MOVE(C) C(C&) = delete; C& operator=(C&) = delete; C(C&&) = delete; C& operator=(C&&) = delete;
