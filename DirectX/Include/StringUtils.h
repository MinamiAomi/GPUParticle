#pragma once

namespace String {

	// wstringに変換
	std::wstring Convert(const std::string& str);
	// wstringから変換
	std::string Convert(const std::wstring& str);

	template<typename ... Args>
	std::string Format(const std::string& format, Args ... args) {
		size_t len = static_cast<size_t>(std::snprintf(nullptr, 0, format.c_str(), args ...));
		std::vector<char> buffer(len + 1);
		std::snprintf(buffer.data(), len + 1, format.c_str(), args ...);
		return std::string(buffer.data(), buffer.data() + len);
	}

};
