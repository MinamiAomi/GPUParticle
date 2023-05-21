#pragma once

class Window final {

public:
	// シングルトンインスタンスを取得
	static Window* GetInstance();

	void Create(const std::string& name = "Window", uint32_t clientWidth = 1280, uint32_t clientHeight = 720);
	void Show();
	void Close();

	inline HWND GetHWND() const { return m_hwnd; }

private:
	// コピー禁止
	Window() = default;
	~Window() = default;
	Window(const Window&) = delete;
	const Window& operator=(const Window&) = delete;

	HWND m_hwnd{ nullptr };
	std::string m_name;
	uint32_t m_clientWidth{ 0 };
	uint32_t m_clientHeight{ 0 };
};

