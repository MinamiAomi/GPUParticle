#pragma once
#include "DirectXHelper.h"

enum class Key : uint8_t {
	_Esc = DIK_ESCAPE,
	_1 = DIK_1,
	_2 = DIK_2,
	_3 = DIK_3,
	_4 = DIK_4,
	_5 = DIK_5,
	_6 = DIK_6,
	_7 = DIK_7,
	_8 = DIK_8,
	_9 = DIK_9,
	_0 = DIK_0,
	_Minus = DIK_MINUS,
	_Equals = DIK_EQUALS,
	_Back = DIK_BACK,
	_Tab = DIK_TAB,
	_Q = DIK_Q,
	_W = DIK_W,
	_E = DIK_E,
	_R = DIK_R,
	_T = DIK_T,
	_Y = DIK_Y,
	_U = DIK_U,
	_I = DIK_I,
	_O = DIK_O,
	_P = DIK_P,
	_LBracket = DIK_LBRACKET,
	_RBracket = DIK_RBRACKET,
	_Enter = DIK_RETURN,
	_LCtrl = DIK_LCONTROL,
	_A = DIK_A,
	_S = DIK_S,
	_D = DIK_D,
	_F = DIK_F,
	_G = DIK_G,
	_H = DIK_H,
	_J = DIK_J,
	_K = DIK_K,
	_L = DIK_L,
	_Semicolon = DIK_SEMICOLON,
	_Apostrophe =  DIK_APOSTROPHE,
	_Grave = DIK_GRAVE,
	_LShift = DIK_LSHIFT,
	//DIK_BACKSLASH,
	//DIK_Z,
	//DIK_X,
	//DIK_C,
	//DIK_V,
	//DIK_B,
	//DIK_N,
	//DIK_M,
	//DIK_COMMA,
	//DIK_PERIOD,
	//DIK_SLASH,
	//DIK_RSHIFT,
	//DIK_MULTIPLY,
	//DIK_LMENU,
	//DIK_SPACE,
	//DIK_CAPITAL,
	//DIK_F1,
	//DIK_F2,
	//DIK_F3,
	//DIK_F4,
	//DIK_F5,
	//DIK_F6,
	//DIK_F7,
	//DIK_F8,
	//DIK_F9,
	//DIK_F10,
	//DIK_NUMLOCK,
	//DIK_SCROLL,
	//DIK_NUMPAD7,
	//DIK_NUMPAD8,
	//DIK_NUMPAD9,
	//DIK_SUBTRACT,
	//DIK_NUMPAD4,
	//DIK_NUMPAD5,
	//DIK_NUMPAD6,
	//DIK_ADD,
	//DIK_NUMPAD1,
	//DIK_NUMPAD2,
	//DIK_NUMPAD3,
	//DIK_NUMPAD0,
	//DIK_DECIMAL,
	//DIK_OEM_102,
	//DIK_F11,
	//DIK_F12,
	//DIK_F13,
	//DIK_F14,
	//DIK_F15,
	//DIK_KANA,
	//DIK_ABNT_C1,
	//DIK_CONVERT,
	//DIK_NOCONVERT,
	//DIK_YEN,
	//DIK_ABNT_C2,
	//DIK_NUMPADEQUALS,
	//DIK_PREVTRACK,
	//DIK_AT,
	//DIK_COLON,
	//DIK_UNDERLINE,
	//DIK_KANJI,
	//DIK_STOP,
	//DIK_AX,
	//DIK_UNLABELED,
	//DIK_NEXTTRACK,
	//DIK_NUMPADENTER,
	//DIK_RCONTROL,
	//DIK_MUTE,
	//DIK_CALCULATOR,
	//DIK_PLAYPAUSE,
	//DIK_MEDIASTOP,
	//DIK_VOLUMEDOWN,
	//DIK_VOLUMEUP,
	//DIK_WEBHOME,
	//DIK_NUMPADCOMMA,
	//DIK_DIVIDE,
	//DIK_SYSRQ,
	//DIK_RMENU,
	//DIK_PAUSE,
	//DIK_HOME,
	//DIK_UP,
	//DIK_PRIOR,
	//DIK_LEFT,
	//DIK_RIGHT,
	//DIK_END,
	//DIK_DOWN,
	//DIK_NEXT,
	//DIK_INSERT,
	//DIK_DELETE,
	//DIK_LWIN,
	//DIK_RWIN,
	//DIK_APPS,
	//DIK_POWER,
	//DIK_SLEEP,
	//DIK_WAKE,
	//DIK_WEBSEARCH,
	//DIK_WEBFAVORITES,
	//DIK_WEBREFRESH,
	//DIK_WEBSTOP,
	//DIK_WEBFORWARD,
	//DIK_WEBBACK,
	//DIK_MYCOMPUTER,
	//DIK_MAIL,
	//DIK_MEDIASELECT,




































};

class Input {
public:
	static void Initalize(HWND hwnd);

private:
	class Impl;
	static Impl impl;
};

