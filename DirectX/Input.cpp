#include "stdafx.h"
#include "Input.h"
#include "MathUtils.h"

class Input::Impl {
private:
	static const uint32_t kKeyCount = 256;

	DirectXHelper::ComPtr<IDirectInput8> directInput_;

	DirectXHelper::ComPtr<IDirectInputDevice8> keyboard_;
	uint8_t curKeys_[kKeyCount]{};
	uint8_t preKeys_[kKeyCount]{};

	DirectXHelper::ComPtr<IDirectInputDevice8> mouse_;
	struct MouseState {
		DIMOUSESTATE2 state;
		Math::Vector2 screenPos;
	};
	MouseState curMouseState_{};
	MouseState preMouseState_{};

public:

	void Initalize(HWND hwnd) {
		assert(hwnd);

		// DirectInputを初期化
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		assert(hInstance);
		CHECK_HRESULT(DirectInput8Create(hInstance, DIRECTINPUT_HEADER_VERSION,
			IID_IDirectInput8, static_cast<void**>(&directInput_), nullptr));
		// キーボード入力を初期化
		CHECK_HRESULT(directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL));
		CHECK_HRESULT(keyboard_->SetDataFormat(&c_dfDIKeyboard));
		CHECK_HRESULT(keyboard_->SetCooperativeLevel(
			hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY));
		// マウス入力を初期化
		CHECK_HRESULT(directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL));
		CHECK_HRESULT(mouse_->SetDataFormat(&c_dfDIMouse));
		CHECK_HRESULT(mouse_->SetCooperativeLevel(
			hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY));

	}
	void Update() {
		memcpy(preKeys_, curKeys_, sizeof(curKeys_));
		CHECK_HRESULT(keyboard_->Acquire());
		keyboard_->GetDeviceState(sizeof(curKeys_), curKeys_);

		preMouseState_ = curMouseState_;
		CHECK_HRESULT(mouse_->Acquire());
		mouse_->GetDeviceState(sizeof(curMouseState_), &curMouseState_);
	}




};

Input::Impl Input::impl;

void Input::Initalize(HWND hwnd) {
	impl.Initalize(hwnd);
}
