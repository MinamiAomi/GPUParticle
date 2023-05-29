#pragma once
#include "DirectXHelper.h"

enum class Key : uint8_t {
	Esc = DIK_ESCAPE,	/* Escape */
	_1 = DIK_1,	// 1
	_2 = DIK_2,	// 2
	_3 = DIK_3,	// 3
	_4 = DIK_4,	// 4
	_5 = DIK_5,	// 5
	_6 = DIK_6,	// 6
	_7 = DIK_7,	// 7
	_8 = DIK_8,	// 8
	_9 = DIK_9,	// 9
	_0 = DIK_0,	// 0
	Minus = DIK_MINUS,	// - (マイナス)
	Equals = DIK_EQUALS,	// = (イコール)
	BackSpace = DIK_BACK,	// BackSpace
	Tab = DIK_TAB,	// Tab
	Q = DIK_Q,	// Q
	W = DIK_W,	// W
	E = DIK_E,	// E
	R = DIK_R,	// R
	T = DIK_T,	// T
	Y = DIK_Y,	// Y
	U = DIK_U,	// U
	I = DIK_I,	// I
	O = DIK_O,	// O
	P = DIK_P,	// P
	LBracket = DIK_LBRACKET,	// [ (左ブラケット)
	RBracket = DIK_RBRACKET,	// ] (右ブラケット)
	Enter = DIK_RETURN,	// Enter
	LCtrl = DIK_LCONTROL,	// Left Ctrl (左コントロール)
	A = DIK_A,	// A	
	S = DIK_S,	// S
	D = DIK_D,	// D
	F = DIK_F,	// F
	G = DIK_G,	// G
	H = DIK_H,	// H
	J = DIK_J,	// J
	K = DIK_K,	// K
	L = DIK_L,	// L
	Semicolon = DIK_SEMICOLON,	// ; (セミコロン)
	Apostrophe = DIK_APOSTROPHE,	// ' (シングルクォーテーション)
	Grave = DIK_GRAVE,	// ` (グライヴ)
	LShift = DIK_LSHIFT,	// Left Shift (左シフト)
	BackSlash = DIK_BACKSLASH,	// \ (バックスラッシュ)
	Z = DIK_Z,	// Z 
	X = DIK_X,	// X 
	C = DIK_C,	// C
	V = DIK_V,	// V
	B = DIK_B,	// B
	N = DIK_N,	// N
	M = DIK_M,	// M
	Comma = DIK_COMMA,	// , (カンマ)
	Period = DIK_PERIOD,	// . (ピリオド)
	Slash = DIK_SLASH,	// / (スラッシュ)
	RShift = DIK_RSHIFT,	// Right Shift (右シフト)
	NumPadMultiply = DIK_MULTIPLY,
	LAlt = DIK_LMENU,	// Left Alt (左オルト)
	Space = DIK_SPACE,	// Space (スペース)
	CapsLock = DIK_CAPITAL,
	F1 = DIK_F1,	// F1 
	F2 = DIK_F2,	// F2 
	F3 = DIK_F3,	// F3 
	F4 = DIK_F4,	// F4 
	F5 = DIK_F5,	// F5 
	F6 = DIK_F6,	// F6 
	F7 = DIK_F7,	// F7 
	F8 = DIK_F8,	// F8 
	F9 = DIK_F9,	// F9 
	F10 = DIK_F10,	// F10
	NumLock = DIK_NUMLOCK,
	ScrollLock = DIK_SCROLL,
	NumPad7 = DIK_NUMPAD7,
	NumPad8 = DIK_NUMPAD8,
	NumPad9 = DIK_NUMPAD9,
	NumPadSubtract = DIK_SUBTRACT,
	NumPad4 = DIK_NUMPAD4,
	NumPad5 = DIK_NUMPAD5,
	NumPad6 = DIK_NUMPAD6,
	NumPadAdd = DIK_ADD,
	NumPad1 = DIK_NUMPAD1,
	NumPad2 = DIK_NUMPAD2,
	NumPad3 = DIK_NUMPAD3,
	NumPad0 = DIK_NUMPAD0,
	NumPadDecimal = DIK_DECIMAL,
	F11 = DIK_F11,
	F12 = DIK_F12,
	F13 = DIK_F13,
	F14 = DIK_F14,
	F15 = DIK_F15,
	Kana = DIK_KANA,
	Convert = DIK_CONVERT,
	NoConvert = DIK_NOCONVERT,
	Yen = DIK_YEN,
	NumPadEquals = DIK_NUMPADEQUALS,
	PrevTrack = DIK_PREVTRACK,
	At = DIK_AT,	// @ (アットマーク)
	Colon = DIK_COLON,	// : (コロン)
	UnderLine = DIK_UNDERLINE,	// _ (アンダーライン)
	Kanji = DIK_KANJI,
	Stop = DIK_STOP,
	AX = DIK_AX,
	Unlabeled = DIK_UNLABELED,
	NextTrack = DIK_NEXTTRACK,
	NumPadEnter = DIK_NUMPADENTER,
	RCtrl = DIK_RCONTROL,	// Right Ctrl (右コントロール)
	Mute = DIK_MUTE,
	Calculator = DIK_CALCULATOR,
	PlayPause = DIK_PLAYPAUSE,
	MediaStop = DIK_MEDIASTOP,
	VolumeDown = DIK_VOLUMEDOWN,
	VolumeUp = DIK_VOLUMEUP,
	WebHome = DIK_WEBHOME,
	NumPadComma = DIK_NUMPADCOMMA,
	NumPadDivide = DIK_DIVIDE,
	SysRq = DIK_SYSRQ,
	RAlt = DIK_RMENU,
	Pause = DIK_PAUSE,
	Home = DIK_HOME,
	Up = DIK_UP,	// ↑ (上矢印)
	PageUp = DIK_PRIOR,
	Left = DIK_LEFT,	// ← (左矢印)
	Right = DIK_RIGHT,	// → (右矢印)
	End = DIK_END,
	Down = DIK_DOWN,	// ↓ (下矢印)
	PageDown = DIK_NEXT,
	Insert = DIK_INSERT,
	Delete = DIK_DELETE,	// Delete (デリート)
	LWindows = DIK_LWIN,
	RWindows = DIK_RWIN,
	Menu = DIK_APPS,
	Power = DIK_POWER,
	Sleep = DIK_SLEEP,
};

class Input {
public:
	static void Initalize(HWND hwnd);

private:
	class Impl;
	static Impl impl;
};

