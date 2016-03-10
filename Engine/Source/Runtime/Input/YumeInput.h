//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeInput_h__
#define __YumeInput_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Math/YumeVector4.h"
#include "Core/YumeVariant.h"
#include "Core/YumeEventHub.h"
#include "Core/YumeTimer.h"
#include <SDL_joystick.h>
#include <SDL_gamecontroller.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	const IntVector2 MOUSE_POSITION_OFFSCREEN = IntVector2(M_MIN_INT,M_MIN_INT);

	enum MouseMode
	{
		MM_ABSOLUTE = 0,
		MM_RELATIVE,
		MM_WRAP,
		MM_FREE
	};


	static const int MOUSEB_LEFT = SDL_BUTTON_LMASK;
	static const int MOUSEB_MIDDLE = SDL_BUTTON_MMASK;
	static const int MOUSEB_RIGHT = SDL_BUTTON_RMASK;
	static const int MOUSEB_X1 = SDL_BUTTON_X1MASK;
	static const int MOUSEB_X2 = SDL_BUTTON_X2MASK;

	static const int QUAL_SHIFT = 1;
	static const int QUAL_CTRL = 2;
	static const int QUAL_ALT = 4;
	static const int QUAL_ANY = 8;

	static const int KEY_A = 'A';
	static const int KEY_B = 'B';
	static const int KEY_C = 'C';
	static const int KEY_D = 'D';
	static const int KEY_E = 'E';
	static const int KEY_F = 'F';
	static const int KEY_G = 'G';
	static const int KEY_H = 'H';
	static const int KEY_I = 'I';
	static const int KEY_J = 'J';
	static const int KEY_K = 'K';
	static const int KEY_L = 'L';
	static const int KEY_M = 'M';
	static const int KEY_N = 'N';
	static const int KEY_O = 'O';
	static const int KEY_P = 'P';
	static const int KEY_Q = 'Q';
	static const int KEY_R = 'R';
	static const int KEY_S = 'S';
	static const int KEY_T = 'T';
	static const int KEY_U = 'U';
	static const int KEY_V = 'V';
	static const int KEY_W = 'W';
	static const int KEY_X = 'X';
	static const int KEY_Y = 'Y';
	static const int KEY_Z = 'Z';
	static const int KEY_0 = '0';
	static const int KEY_1 = '1';
	static const int KEY_2 = '2';
	static const int KEY_3 = '3';
	static const int KEY_4 = '4';
	static const int KEY_5 = '5';
	static const int KEY_6 = '6';
	static const int KEY_7 = '7';
	static const int KEY_8 = '8';
	static const int KEY_9 = '9';
	static const int KEY_BACKSPACE = SDLK_BACKSPACE;
	static const int KEY_TAB = SDLK_TAB;
	static const int KEY_RETURN = SDLK_RETURN;
	static const int KEY_RETURN2 = SDLK_RETURN2;
	static const int KEY_KP_ENTER = SDLK_KP_ENTER;
	static const int KEY_SHIFT = SDLK_LSHIFT;
	static const int KEY_CTRL = SDLK_LCTRL;
	static const int KEY_ALT = SDLK_LALT;
	static const int KEY_GUI = SDLK_LGUI;
	static const int KEY_PAUSE = SDLK_PAUSE;
	static const int KEY_CAPSLOCK = SDLK_CAPSLOCK;
	static const int KEY_ESC = SDLK_ESCAPE;
	static const int KEY_SPACE = SDLK_SPACE;
	static const int KEY_PAGEUP = SDLK_PAGEUP;
	static const int KEY_PAGEDOWN = SDLK_PAGEDOWN;
	static const int KEY_END = SDLK_END;
	static const int KEY_HOME = SDLK_HOME;
	static const int KEY_LEFT = SDLK_LEFT;
	static const int KEY_UP = SDLK_UP;
	static const int KEY_RIGHT = SDLK_RIGHT;
	static const int KEY_DOWN = SDLK_DOWN;
	static const int KEY_SELECT = SDLK_SELECT;
	static const int KEY_PRINTSCREEN = SDLK_PRINTSCREEN;
	static const int KEY_INSERT = SDLK_INSERT;
	static const int KEY_DELETE = SDLK_DELETE;
	static const int KEY_LGUI = SDLK_LGUI;
	static const int KEY_RGUI = SDLK_RGUI;
	static const int KEY_APPLICATION = SDLK_APPLICATION;
	static const int KEY_KP_0 = SDLK_KP_0;
	static const int KEY_KP_1 = SDLK_KP_1;
	static const int KEY_KP_2 = SDLK_KP_2;
	static const int KEY_KP_3 = SDLK_KP_3;
	static const int KEY_KP_4 = SDLK_KP_4;
	static const int KEY_KP_5 = SDLK_KP_5;
	static const int KEY_KP_6 = SDLK_KP_6;
	static const int KEY_KP_7 = SDLK_KP_7;
	static const int KEY_KP_8 = SDLK_KP_8;
	static const int KEY_KP_9 = SDLK_KP_9;
	static const int KEY_KP_MULTIPLY = SDLK_KP_MULTIPLY;
	static const int KEY_KP_PLUS = SDLK_KP_PLUS;
	static const int KEY_KP_MINUS = SDLK_KP_MINUS;
	static const int KEY_KP_PERIOD = SDLK_KP_PERIOD;
	static const int KEY_KP_DIVIDE = SDLK_KP_DIVIDE;
	static const int KEY_F1 = SDLK_F1;
	static const int KEY_F2 = SDLK_F2;
	static const int KEY_F3 = SDLK_F3;
	static const int KEY_F4 = SDLK_F4;
	static const int KEY_F5 = SDLK_F5;
	static const int KEY_F6 = SDLK_F6;
	static const int KEY_F7 = SDLK_F7;
	static const int KEY_F8 = SDLK_F8;
	static const int KEY_F9 = SDLK_F9;
	static const int KEY_F10 = SDLK_F10;
	static const int KEY_F11 = SDLK_F11;
	static const int KEY_F12 = SDLK_F12;
	static const int KEY_F13 = SDLK_F13;
	static const int KEY_F14 = SDLK_F14;
	static const int KEY_F15 = SDLK_F15;
	static const int KEY_F16 = SDLK_F16;
	static const int KEY_F17 = SDLK_F17;
	static const int KEY_F18 = SDLK_F18;
	static const int KEY_F19 = SDLK_F19;
	static const int KEY_F20 = SDLK_F20;
	static const int KEY_F21 = SDLK_F21;
	static const int KEY_F22 = SDLK_F22;
	static const int KEY_F23 = SDLK_F23;
	static const int KEY_F24 = SDLK_F24;
	static const int KEY_NUMLOCKCLEAR = SDLK_NUMLOCKCLEAR;
	static const int KEY_SCROLLLOCK = SDLK_SCROLLLOCK;
	static const int KEY_LSHIFT = SDLK_LSHIFT;
	static const int KEY_RSHIFT = SDLK_RSHIFT;
	static const int KEY_LCTRL = SDLK_LCTRL;
	static const int KEY_RCTRL = SDLK_RCTRL;
	static const int KEY_LALT = SDLK_LALT;
	static const int KEY_RALT = SDLK_RALT;


	static const int SCANCODE_UNKNOWN = SDL_SCANCODE_UNKNOWN;
	static const int SCANCODE_CTRL = SDL_SCANCODE_LCTRL;
	static const int SCANCODE_SHIFT = SDL_SCANCODE_LSHIFT;
	static const int SCANCODE_ALT = SDL_SCANCODE_LALT;
	static const int SCANCODE_GUI = SDL_SCANCODE_LGUI;
	static const int SCANCODE_A = SDL_SCANCODE_A;
	static const int SCANCODE_B = SDL_SCANCODE_B;
	static const int SCANCODE_C = SDL_SCANCODE_C;
	static const int SCANCODE_D = SDL_SCANCODE_D;
	static const int SCANCODE_E = SDL_SCANCODE_E;
	static const int SCANCODE_F = SDL_SCANCODE_F;
	static const int SCANCODE_G = SDL_SCANCODE_G;
	static const int SCANCODE_H = SDL_SCANCODE_H;
	static const int SCANCODE_I = SDL_SCANCODE_I;
	static const int SCANCODE_J = SDL_SCANCODE_J;
	static const int SCANCODE_K = SDL_SCANCODE_K;
	static const int SCANCODE_L = SDL_SCANCODE_L;
	static const int SCANCODE_M = SDL_SCANCODE_M;
	static const int SCANCODE_N = SDL_SCANCODE_N;
	static const int SCANCODE_O = SDL_SCANCODE_O;
	static const int SCANCODE_P = SDL_SCANCODE_P;
	static const int SCANCODE_Q = SDL_SCANCODE_Q;
	static const int SCANCODE_R = SDL_SCANCODE_R;
	static const int SCANCODE_S = SDL_SCANCODE_S;
	static const int SCANCODE_T = SDL_SCANCODE_T;
	static const int SCANCODE_U = SDL_SCANCODE_U;
	static const int SCANCODE_V = SDL_SCANCODE_V;
	static const int SCANCODE_W = SDL_SCANCODE_W;
	static const int SCANCODE_X = SDL_SCANCODE_X;
	static const int SCANCODE_Y = SDL_SCANCODE_Y;
	static const int SCANCODE_Z = SDL_SCANCODE_Z;
	static const int SCANCODE_1 = SDL_SCANCODE_1;
	static const int SCANCODE_2 = SDL_SCANCODE_2;
	static const int SCANCODE_3 = SDL_SCANCODE_3;
	static const int SCANCODE_4 = SDL_SCANCODE_4;
	static const int SCANCODE_5 = SDL_SCANCODE_5;
	static const int SCANCODE_6 = SDL_SCANCODE_6;
	static const int SCANCODE_7 = SDL_SCANCODE_7;
	static const int SCANCODE_8 = SDL_SCANCODE_8;
	static const int SCANCODE_9 = SDL_SCANCODE_9;
	static const int SCANCODE_0 = SDL_SCANCODE_0;
	static const int SCANCODE_RETURN = SDL_SCANCODE_RETURN;
	static const int SCANCODE_ESCAPE = SDL_SCANCODE_ESCAPE;
	static const int SCANCODE_BACKSPACE = SDL_SCANCODE_BACKSPACE;
	static const int SCANCODE_TAB = SDL_SCANCODE_TAB;
	static const int SCANCODE_SPACE = SDL_SCANCODE_SPACE;
	static const int SCANCODE_MINUS = SDL_SCANCODE_MINUS;
	static const int SCANCODE_EQUALS = SDL_SCANCODE_EQUALS;
	static const int SCANCODE_LEFTBRACKET = SDL_SCANCODE_LEFTBRACKET;
	static const int SCANCODE_RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET;
	static const int SCANCODE_BACKSLASH = SDL_SCANCODE_BACKSLASH;
	static const int SCANCODE_NONUSHASH = SDL_SCANCODE_NONUSHASH;
	static const int SCANCODE_SEMICOLON = SDL_SCANCODE_SEMICOLON;
	static const int SCANCODE_APOSTROPHE = SDL_SCANCODE_APOSTROPHE;
	static const int SCANCODE_GRAVE = SDL_SCANCODE_GRAVE;
	static const int SCANCODE_COMMA = SDL_SCANCODE_COMMA;
	static const int SCANCODE_PERIOD = SDL_SCANCODE_PERIOD;
	static const int SCANCODE_SLASH = SDL_SCANCODE_SLASH;
	static const int SCANCODE_CAPSLOCK = SDL_SCANCODE_CAPSLOCK;
	static const int SCANCODE_F1 = SDL_SCANCODE_F1;
	static const int SCANCODE_F2 = SDL_SCANCODE_F2;
	static const int SCANCODE_F3 = SDL_SCANCODE_F3;
	static const int SCANCODE_F4 = SDL_SCANCODE_F4;
	static const int SCANCODE_F5 = SDL_SCANCODE_F5;
	static const int SCANCODE_F6 = SDL_SCANCODE_F6;
	static const int SCANCODE_F7 = SDL_SCANCODE_F7;
	static const int SCANCODE_F8 = SDL_SCANCODE_F8;
	static const int SCANCODE_F9 = SDL_SCANCODE_F9;
	static const int SCANCODE_F10 = SDL_SCANCODE_F10;
	static const int SCANCODE_F11 = SDL_SCANCODE_F11;
	static const int SCANCODE_F12 = SDL_SCANCODE_F12;
	static const int SCANCODE_PRINTSCREEN = SDL_SCANCODE_PRINTSCREEN;
	static const int SCANCODE_SCROLLLOCK = SDL_SCANCODE_SCROLLLOCK;
	static const int SCANCODE_PAUSE = SDL_SCANCODE_PAUSE;
	static const int SCANCODE_INSERT = SDL_SCANCODE_INSERT;
	static const int SCANCODE_HOME = SDL_SCANCODE_HOME;
	static const int SCANCODE_PAGEUP = SDL_SCANCODE_PAGEUP;
	static const int SCANCODE_DELETE = SDL_SCANCODE_DELETE;
	static const int SCANCODE_END = SDL_SCANCODE_END;
	static const int SCANCODE_PAGEDOWN = SDL_SCANCODE_PAGEDOWN;
	static const int SCANCODE_RIGHT = SDL_SCANCODE_RIGHT;
	static const int SCANCODE_LEFT = SDL_SCANCODE_LEFT;
	static const int SCANCODE_DOWN = SDL_SCANCODE_DOWN;
	static const int SCANCODE_UP = SDL_SCANCODE_UP;
	static const int SCANCODE_NUMLOCKCLEAR = SDL_SCANCODE_NUMLOCKCLEAR;
	static const int SCANCODE_KP_DIVIDE = SDL_SCANCODE_KP_DIVIDE;
	static const int SCANCODE_KP_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY;
	static const int SCANCODE_KP_MINUS = SDL_SCANCODE_KP_MINUS;
	static const int SCANCODE_KP_PLUS = SDL_SCANCODE_KP_PLUS;
	static const int SCANCODE_KP_ENTER = SDL_SCANCODE_KP_ENTER;
	static const int SCANCODE_KP_1 = SDL_SCANCODE_KP_1;
	static const int SCANCODE_KP_2 = SDL_SCANCODE_KP_2;
	static const int SCANCODE_KP_3 = SDL_SCANCODE_KP_3;
	static const int SCANCODE_KP_4 = SDL_SCANCODE_KP_4;
	static const int SCANCODE_KP_5 = SDL_SCANCODE_KP_5;
	static const int SCANCODE_KP_6 = SDL_SCANCODE_KP_6;
	static const int SCANCODE_KP_7 = SDL_SCANCODE_KP_7;
	static const int SCANCODE_KP_8 = SDL_SCANCODE_KP_8;
	static const int SCANCODE_KP_9 = SDL_SCANCODE_KP_9;
	static const int SCANCODE_KP_0 = SDL_SCANCODE_KP_0;
	static const int SCANCODE_KP_PERIOD = SDL_SCANCODE_KP_PERIOD;
	static const int SCANCODE_NONUSBACKSLASH = SDL_SCANCODE_NONUSBACKSLASH;
	static const int SCANCODE_APPLICATION = SDL_SCANCODE_APPLICATION;
	static const int SCANCODE_POWER = SDL_SCANCODE_POWER;
	static const int SCANCODE_KP_EQUALS = SDL_SCANCODE_KP_EQUALS;
	static const int SCANCODE_F13 = SDL_SCANCODE_F13;
	static const int SCANCODE_F14 = SDL_SCANCODE_F14;
	static const int SCANCODE_F15 = SDL_SCANCODE_F15;
	static const int SCANCODE_F16 = SDL_SCANCODE_F16;
	static const int SCANCODE_F17 = SDL_SCANCODE_F17;
	static const int SCANCODE_F18 = SDL_SCANCODE_F18;
	static const int SCANCODE_F19 = SDL_SCANCODE_F19;
	static const int SCANCODE_F20 = SDL_SCANCODE_F20;
	static const int SCANCODE_F21 = SDL_SCANCODE_F21;
	static const int SCANCODE_F22 = SDL_SCANCODE_F22;
	static const int SCANCODE_F23 = SDL_SCANCODE_F23;
	static const int SCANCODE_F24 = SDL_SCANCODE_F24;
	static const int SCANCODE_EXECUTE = SDL_SCANCODE_EXECUTE;
	static const int SCANCODE_HELP = SDL_SCANCODE_HELP;
	static const int SCANCODE_MENU = SDL_SCANCODE_MENU;
	static const int SCANCODE_SELECT = SDL_SCANCODE_SELECT;
	static const int SCANCODE_STOP = SDL_SCANCODE_STOP;
	static const int SCANCODE_AGAIN = SDL_SCANCODE_AGAIN;
	static const int SCANCODE_UNDO = SDL_SCANCODE_UNDO;
	static const int SCANCODE_CUT = SDL_SCANCODE_CUT;
	static const int SCANCODE_COPY = SDL_SCANCODE_COPY;
	static const int SCANCODE_PASTE = SDL_SCANCODE_PASTE;
	static const int SCANCODE_FIND = SDL_SCANCODE_FIND;
	static const int SCANCODE_MUTE = SDL_SCANCODE_MUTE;
	static const int SCANCODE_VOLUMEUP = SDL_SCANCODE_VOLUMEUP;
	static const int SCANCODE_VOLUMEDOWN = SDL_SCANCODE_VOLUMEDOWN;
	static const int SCANCODE_KP_COMMA = SDL_SCANCODE_KP_COMMA;
	static const int SCANCODE_KP_EQUALSAS400 = SDL_SCANCODE_KP_EQUALSAS400;
	static const int SCANCODE_INTERNATIONAL1 = SDL_SCANCODE_INTERNATIONAL1;
	static const int SCANCODE_INTERNATIONAL2 = SDL_SCANCODE_INTERNATIONAL2;
	static const int SCANCODE_INTERNATIONAL3 = SDL_SCANCODE_INTERNATIONAL3;
	static const int SCANCODE_INTERNATIONAL4 = SDL_SCANCODE_INTERNATIONAL4;
	static const int SCANCODE_INTERNATIONAL5 = SDL_SCANCODE_INTERNATIONAL5;
	static const int SCANCODE_INTERNATIONAL6 = SDL_SCANCODE_INTERNATIONAL6;
	static const int SCANCODE_INTERNATIONAL7 = SDL_SCANCODE_INTERNATIONAL7;
	static const int SCANCODE_INTERNATIONAL8 = SDL_SCANCODE_INTERNATIONAL8;
	static const int SCANCODE_INTERNATIONAL9 = SDL_SCANCODE_INTERNATIONAL9;
	static const int SCANCODE_LANG1 = SDL_SCANCODE_LANG1;
	static const int SCANCODE_LANG2 = SDL_SCANCODE_LANG2;
	static const int SCANCODE_LANG3 = SDL_SCANCODE_LANG3;
	static const int SCANCODE_LANG4 = SDL_SCANCODE_LANG4;
	static const int SCANCODE_LANG5 = SDL_SCANCODE_LANG5;
	static const int SCANCODE_LANG6 = SDL_SCANCODE_LANG6;
	static const int SCANCODE_LANG7 = SDL_SCANCODE_LANG7;
	static const int SCANCODE_LANG8 = SDL_SCANCODE_LANG8;
	static const int SCANCODE_LANG9 = SDL_SCANCODE_LANG9;
	static const int SCANCODE_ALTERASE = SDL_SCANCODE_ALTERASE;
	static const int SCANCODE_SYSREQ = SDL_SCANCODE_SYSREQ;
	static const int SCANCODE_CANCEL = SDL_SCANCODE_CANCEL;
	static const int SCANCODE_CLEAR = SDL_SCANCODE_CLEAR;
	static const int SCANCODE_PRIOR = SDL_SCANCODE_PRIOR;
	static const int SCANCODE_RETURN2 = SDL_SCANCODE_RETURN2;
	static const int SCANCODE_SEPARATOR = SDL_SCANCODE_SEPARATOR;
	static const int SCANCODE_OUT = SDL_SCANCODE_OUT;
	static const int SCANCODE_OPER = SDL_SCANCODE_OPER;
	static const int SCANCODE_CLEARAGAIN = SDL_SCANCODE_CLEARAGAIN;
	static const int SCANCODE_CRSEL = SDL_SCANCODE_CRSEL;
	static const int SCANCODE_EXSEL = SDL_SCANCODE_EXSEL;
	static const int SCANCODE_KP_00 = SDL_SCANCODE_KP_00;
	static const int SCANCODE_KP_000 = SDL_SCANCODE_KP_000;
	static const int SCANCODE_THOUSANDSSEPARATOR = SDL_SCANCODE_THOUSANDSSEPARATOR;
	static const int SCANCODE_DECIMALSEPARATOR = SDL_SCANCODE_DECIMALSEPARATOR;
	static const int SCANCODE_CURRENCYUNIT = SDL_SCANCODE_CURRENCYUNIT;
	static const int SCANCODE_CURRENCYSUBUNIT = SDL_SCANCODE_CURRENCYSUBUNIT;
	static const int SCANCODE_KP_LEFTPAREN = SDL_SCANCODE_KP_LEFTPAREN;
	static const int SCANCODE_KP_RIGHTPAREN = SDL_SCANCODE_KP_RIGHTPAREN;
	static const int SCANCODE_KP_LEFTBRACE = SDL_SCANCODE_KP_LEFTBRACE;
	static const int SCANCODE_KP_RIGHTBRACE = SDL_SCANCODE_KP_RIGHTBRACE;
	static const int SCANCODE_KP_TAB = SDL_SCANCODE_KP_TAB;
	static const int SCANCODE_KP_BACKSPACE = SDL_SCANCODE_KP_BACKSPACE;
	static const int SCANCODE_KP_A = SDL_SCANCODE_KP_A;
	static const int SCANCODE_KP_B = SDL_SCANCODE_KP_B;
	static const int SCANCODE_KP_C = SDL_SCANCODE_KP_C;
	static const int SCANCODE_KP_D = SDL_SCANCODE_KP_D;
	static const int SCANCODE_KP_E = SDL_SCANCODE_KP_E;
	static const int SCANCODE_KP_F = SDL_SCANCODE_KP_F;
	static const int SCANCODE_KP_XOR = SDL_SCANCODE_KP_XOR;
	static const int SCANCODE_KP_POWER = SDL_SCANCODE_KP_POWER;
	static const int SCANCODE_KP_PERCENT = SDL_SCANCODE_KP_PERCENT;
	static const int SCANCODE_KP_LESS = SDL_SCANCODE_KP_LESS;
	static const int SCANCODE_KP_GREATER = SDL_SCANCODE_KP_GREATER;
	static const int SCANCODE_KP_AMPERSAND = SDL_SCANCODE_KP_AMPERSAND;
	static const int SCANCODE_KP_DBLAMPERSAND = SDL_SCANCODE_KP_DBLAMPERSAND;
	static const int SCANCODE_KP_VERTICALBAR = SDL_SCANCODE_KP_VERTICALBAR;
	static const int SCANCODE_KP_DBLVERTICALBAR = SDL_SCANCODE_KP_DBLVERTICALBAR;
	static const int SCANCODE_KP_COLON = SDL_SCANCODE_KP_COLON;
	static const int SCANCODE_KP_HASH = SDL_SCANCODE_KP_HASH;
	static const int SCANCODE_KP_SPACE = SDL_SCANCODE_KP_SPACE;
	static const int SCANCODE_KP_AT = SDL_SCANCODE_KP_AT;
	static const int SCANCODE_KP_EXCLAM = SDL_SCANCODE_KP_EXCLAM;
	static const int SCANCODE_KP_MEMSTORE = SDL_SCANCODE_KP_MEMSTORE;
	static const int SCANCODE_KP_MEMRECALL = SDL_SCANCODE_KP_MEMRECALL;
	static const int SCANCODE_KP_MEMCLEAR = SDL_SCANCODE_KP_MEMCLEAR;
	static const int SCANCODE_KP_MEMADD = SDL_SCANCODE_KP_MEMADD;
	static const int SCANCODE_KP_MEMSUBTRACT = SDL_SCANCODE_KP_MEMSUBTRACT;
	static const int SCANCODE_KP_MEMMULTIPLY = SDL_SCANCODE_KP_MEMMULTIPLY;
	static const int SCANCODE_KP_MEMDIVIDE = SDL_SCANCODE_KP_MEMDIVIDE;
	static const int SCANCODE_KP_PLUSMINUS = SDL_SCANCODE_KP_PLUSMINUS;
	static const int SCANCODE_KP_CLEAR = SDL_SCANCODE_KP_CLEAR;
	static const int SCANCODE_KP_CLEARENTRY = SDL_SCANCODE_KP_CLEARENTRY;
	static const int SCANCODE_KP_BINARY = SDL_SCANCODE_KP_BINARY;
	static const int SCANCODE_KP_OCTAL = SDL_SCANCODE_KP_OCTAL;
	static const int SCANCODE_KP_DECIMAL = SDL_SCANCODE_KP_DECIMAL;
	static const int SCANCODE_KP_HEXADECIMAL = SDL_SCANCODE_KP_HEXADECIMAL;
	static const int SCANCODE_LCTRL = SDL_SCANCODE_LCTRL;
	static const int SCANCODE_LSHIFT = SDL_SCANCODE_LSHIFT;
	static const int SCANCODE_LALT = SDL_SCANCODE_LALT;
	static const int SCANCODE_LGUI = SDL_SCANCODE_LGUI;
	static const int SCANCODE_RCTRL = SDL_SCANCODE_RCTRL;
	static const int SCANCODE_RSHIFT = SDL_SCANCODE_RSHIFT;
	static const int SCANCODE_RALT = SDL_SCANCODE_RALT;
	static const int SCANCODE_RGUI = SDL_SCANCODE_RGUI;
	static const int SCANCODE_MODE = SDL_SCANCODE_MODE;
	static const int SCANCODE_AUDIONEXT = SDL_SCANCODE_AUDIONEXT;
	static const int SCANCODE_AUDIOPREV = SDL_SCANCODE_AUDIOPREV;
	static const int SCANCODE_AUDIOSTOP = SDL_SCANCODE_AUDIOSTOP;
	static const int SCANCODE_AUDIOPLAY = SDL_SCANCODE_AUDIOPLAY;
	static const int SCANCODE_AUDIOMUTE = SDL_SCANCODE_AUDIOMUTE;
	static const int SCANCODE_MEDIASELECT = SDL_SCANCODE_MEDIASELECT;
	static const int SCANCODE_WWW = SDL_SCANCODE_WWW;
	static const int SCANCODE_MAIL = SDL_SCANCODE_MAIL;
	static const int SCANCODE_CALCULATOR = SDL_SCANCODE_CALCULATOR;
	static const int SCANCODE_COMPUTER = SDL_SCANCODE_COMPUTER;
	static const int SCANCODE_AC_SEARCH = SDL_SCANCODE_AC_SEARCH;
	static const int SCANCODE_AC_HOME = SDL_SCANCODE_AC_HOME;
	static const int SCANCODE_AC_BACK = SDL_SCANCODE_AC_BACK;
	static const int SCANCODE_AC_FORWARD = SDL_SCANCODE_AC_FORWARD;
	static const int SCANCODE_AC_STOP = SDL_SCANCODE_AC_STOP;
	static const int SCANCODE_AC_REFRESH = SDL_SCANCODE_AC_REFRESH;
	static const int SCANCODE_AC_BOOKMARKS = SDL_SCANCODE_AC_BOOKMARKS;
	static const int SCANCODE_BRIGHTNESSDOWN = SDL_SCANCODE_BRIGHTNESSDOWN;
	static const int SCANCODE_BRIGHTNESSUP = SDL_SCANCODE_BRIGHTNESSUP;
	static const int SCANCODE_DISPLAYSWITCH = SDL_SCANCODE_DISPLAYSWITCH;
	static const int SCANCODE_KBDILLUMTOGGLE = SDL_SCANCODE_KBDILLUMTOGGLE;
	static const int SCANCODE_KBDILLUMDOWN = SDL_SCANCODE_KBDILLUMDOWN;
	static const int SCANCODE_KBDILLUMUP = SDL_SCANCODE_KBDILLUMUP;
	static const int SCANCODE_EJECT = SDL_SCANCODE_EJECT;
	static const int SCANCODE_SLEEP = SDL_SCANCODE_SLEEP;
	static const int SCANCODE_APP1 = SDL_SCANCODE_APP1;
	static const int SCANCODE_APP2 = SDL_SCANCODE_APP2;

	class YumeRHI;
	

	class YumeAPIExport YumeInput : public YumeTimerEventListener,public RHIEventListener
	{
	public:
		YumeInput();
		virtual ~YumeInput();


		void Update();

		void ToggleFullscreen(bool b);
		void SetMouseVisible(bool enable,bool suppressEvent = false);
		void ResetMouseVisible();
		void SetMouseGrabbed(bool grab);
		void SetMouseMode(MouseMode mode);


		YumeString GetKeyName(int key) const;
		int GetKeyFromName(const YumeString& name) const;

		bool GetMouseButtonDown(int button) const;
		bool GetMouseButtonPress(int button) const;

		bool GetKeyDown(int key) const;
		bool GetKeyPress(int key) const;

		IntVector2 GetMousePosition() const;

		const IntVector2& GetMouseMove() const { return mouseMove_; }

		int GetMouseMoveX() const { return mouseMove_.x_; }
		int GetMouseMoveY() const { return mouseMove_.y_; }
		int GetMouseMoveWheel() const { return mouseMoveWheel_; }

		bool GetToggleFullscreen() const { return toggleFullscreen_; }

		bool IsMouseVisible() const { return mouseVisible_; }

		bool IsMouseGrabbed() const { return mouseGrabbed_; }
		MouseMode GetMouseMode() const { return mouseMode_; }
		bool HasFocus() { return inputFocus_; }
		bool IsMinimized() const;

		void Initialize();

		void GainFocus();
		void LoseFocus();
		void ResetState();

		void SetMouseButton(int button,bool newState);
		void SetKey(int key,int scancode,unsigned raw,bool newState);

		void SetMouseWheel(int delta);
		void SetMousePosition(const IntVector2& position);
		void HandleScreenMode(YumeHash eventType,VariantMap& eventData);
		void HandleBeginFrame(int frameNumber);

		void HandleSDLEvent(void* sdlEvent);
		void SendInputFocusEvent();

	private:
		YumeVector<int>::type keyDown_;
		YumeVector<int>::type keyPress_;

		SharedPtr<YumeRHI> graphics_;


		unsigned mouseButtonDown_;
		unsigned mouseButtonPress_;
		IntVector2 lastMousePosition_;
		IntVector2 lastVisibleMousePosition_;
		IntVector2 mouseMove_;
		int mouseMoveWheel_;
		unsigned windowID_;
		bool toggleFullscreen_;
		bool mouseVisible_;
		bool lastMouseVisible_;
		bool mouseGrabbed_;
		MouseMode mouseMode_;

		bool inputFocus_;
		bool minimized_;
		bool focusedThisFrame_;
		bool suppressNextMouseMove_;
		bool inResize_;
		bool screenModeChanged_;
		bool initialized_;
	};
}


//----------------------------------------------------------------------------
#endif
