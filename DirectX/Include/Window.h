#pragma once

class Window final {
public:
	Window() = default;
	DELETE_COPY_MOVE(Window);

	void Create(const std::string& name = "Window", uint32_t clientWidth = 1280, uint32_t clientHeight = 720);
	void Show();
	void Close();

	inline HWND GetHWND() const { return hwnd_; }
	inline uint32_t GetClientWidth() const { return clientWidth_; }
	inline uint32_t GetClientHeight() const { return clientHeight_; }

private:
	HWND hwnd_{ nullptr };
	std::string name_;
	uint32_t clientWidth_{ 0 };
	uint32_t clientHeight_{ 0 };
};

