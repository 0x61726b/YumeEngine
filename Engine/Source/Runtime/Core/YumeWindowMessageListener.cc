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

#include "YumeHeaders.h"
#include "Core/YumeWindowMessageListener.h"
#include "Graphics/YumeRenderWindow.h"
#include "Core/YumeCentrum.h"
#include "Core/YumeStringConverter.h"

#if YUME_PLATFORM == YUME_PLATFORM_LINUX
#include <X11/Xlib.h>
void GLXProc(YumeEngine::YumeRenderWindow *win, const XEvent &event);
#endif


YumeEngine::YumeWindowEvents::WindowEventListeners YumeEngine::YumeWindowEvents::_msListeners;
YumeEngine::YumeWindowEvents::Windows YumeEngine::YumeWindowEvents::_msWindows;

namespace YumeEngine
{
	void YumeWindowEvents::Run()
	{
	#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		MSG  msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	#endif
		//linux to come
	}

	void YumeWindowEvents::AddWindowEventListener(YumeRenderWindow* window, YumeWindowEventListener* listener)
	{
		_msListeners.insert(std::make_pair(window, listener));
	}
	void YumeWindowEvents::RemoveWindowEventListener(YumeRenderWindow* window, YumeWindowEventListener* listener)
	{
		WindowEventListeners::iterator i = _msListeners.begin(), e = _msListeners.end();

		for (; i != e; ++i)
		{
			if (i->first == window && i->second == listener)
			{
				_msListeners.erase(i);
				break;
			}
		}
	}
	void YumeWindowEvents::AddRenderWindow(YumeRenderWindow* window)
	{
		_msWindows.push_back(window);
	}
	void YumeWindowEvents::RemoveRenderWindow(YumeRenderWindow* window)
	{
		Windows::iterator i = std::find(_msWindows.begin(), _msWindows.end(), window);
		if (i != _msWindows.end())
			_msWindows.erase(i);
	}
}

namespace YumeEngine
{
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
	LRESULT CALLBACK YumeWindowEvents::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
		{	// Store pointer to Win32Window in user data area
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));
			return 0;
		}

		// look up window instance
		// note: it is possible to get a WM_SIZE before WM_CREATE
		YumeRenderWindow* win = (YumeRenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!win)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

		
		WindowEventListeners::iterator index,
			start = _msListeners.lower_bound(win),
			end = _msListeners.upper_bound(win);

		switch (uMsg)
		{
		case WM_ACTIVATE:
		{
			bool active = (LOWORD(wParam) != WA_INACTIVE);
			if (active)
			{
				win->SetActive(true);
			}
			else
			{
				if (win->IsDeactivatedOnFocusChange())
				{
					win->SetActive(false);
				}
			}

			for (; start != end; ++start)
				(start->second)->OnWindowFocusChange(win);
			break;
		}
		case WM_SYSKEYDOWN:
			switch (wParam)
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
						  //return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSKEYUP:
			switch (wParam)
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
			case VK_F10:
				//return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSCHAR:
			// return zero to bypass defProc and signal we processed the message, unless it's an ALT-space
			if (wParam != VK_SPACE)
				return 0;
			break;
		case WM_ENTERSIZEMOVE:
			//log->logMessage("WM_ENTERSIZEMOVE");
			break;
		case WM_EXITSIZEMOVE:
			//log->logMessage("WM_EXITSIZEMOVE");
			break;
		case WM_MOVE:
			//log->logMessage("WM_MOVE");
			win->OnWindowMovedOrResized();
			for (index = start; index != end; ++index)
				(index->second)->OnWindowMoved(win);
			break;
		case WM_DISPLAYCHANGE:
			win->OnWindowMovedOrResized();
			for (index = start; index != end; ++index)
				(index->second)->OnWindowResized(win);
			break;
		case WM_SIZE:
			//log->logMessage("WM_SIZE");
			win->OnWindowMovedOrResized();
			for (index = start; index != end; ++index)
				(index->second)->OnWindowResized(win);
			break;
		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimu size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;
		case WM_CLOSE:
		{
			//log->logMessage("WM_CLOSE");
			bool close = true;
			for (index = start; index != end; ++index)
			{
				if (!(index->second)->OnWindowClosing(win))
					close = false;
			}
			if (!close) return 0;

			for (index = _msListeners.lower_bound(win); index != end; ++index)
				(index->second)->OnWindowClosed(win);
			win->Destroy();
			return 0;
		}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
#endif
}