///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 arkenthera

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : YumeWindowMessageListener.h
/// Date : 4.9.2015
/// Comments : 
//
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumeWindowMessageListener_h__
#define __YumeWindowMessageListener_h__
///--------------------------------------------------------------------------------
#include "Core/YumeRequired.h"


namespace YumeEngine
{
	class YumeAPIExport YumeWindowEventListener
	{
	public:
		virtual ~YumeWindowEventListener() {}

		virtual void OnWindowMoved(YumeRenderWindow* rw)
		{
			(void)rw;
		}

		virtual void OnWindowresized(YumeRenderWindow* rw)
		{
			(void)rw;
		}

		virtual bool OnWindowClosing(YumeRenderWindow* rw)
		{
			(void)rw; return true;
		}

		virtual void OnWindowClosed(YumeRenderWindow* rw)
		{
			(void)rw;
		}

		virtual void OnWindowFocusChange(YumeRenderWindow* rw)
		{
			(void)rw;
		}
	};

	class YumeAPIExport YumeWindowEvents
	{
	public:
		static void Run();

		static void AddWindowEventListener(YumeRenderWindow* window, YumeWindowEventListener* listener);

		static void RemoveWindowEventListener(YumeRenderWindow* window, YumeWindowEventListener* listener);

	public:
		static void AddRenderWindow(YumeRenderWindow* window);
		static void RemoveRenderWindow(YumeRenderWindow* window);

	public:

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		//! Internal winProc (RenderWindow's use this when creating the Win32 Window)
		static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif YUME_PLATFORM == YUME_PLATFORM_APPLE && !defined __OBJC__ && !defined(__LP64__)
		//! Internal UPP Window Handler (RenderWindow's use this when creating the OS X Carbon Window
		static OSStatus _CarbonWindowHandler(EventHandlerCallRef nextHandler, EventRef event, void* wnd);
#endif

		//These are public only so GLXProc can access them without adding Xlib headers header
		typedef YumeMultiMap<YumeRenderWindow*, YumeWindowEventListener*>::type WindowEventListeners;
		static WindowEventListeners _msListeners;

		typedef YumeVector<YumeRenderWindow*>::type Windows;
		static Windows _msWindows;
	};
}
///--------------------------------------------------------------------------------
//End of __YumeWindowMessageListener_h__
#endif