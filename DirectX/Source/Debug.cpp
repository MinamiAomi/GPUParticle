#include "Debug.h"

namespace Debug {
	void Log(const std::string& str) {
		OutputDebugStringA(str.c_str());
	}
	void Log(const std::wstring& str) {
		OutputDebugStringW(str.c_str());
	}
}
