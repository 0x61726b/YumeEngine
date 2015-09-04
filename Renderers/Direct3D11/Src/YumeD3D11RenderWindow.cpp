///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 Alperen Gezer

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
/// File : XX.h
/// Date : XX.XX.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

#include "YumeD3D11RenderWindow.h"
#include "YumeD3D11Renderer.h"
#include "YumeD3D11Adapter.h"
#include "YumeWindowMessageListener.h"

namespace YumeEngine
{
	///--------------------------------------------------------------------------------
	YumeD3D11RenderWindow::YumeD3D11RenderWindow(HINSTANCE instance, YumeD3D11Device & device, IDXGIFactory1*	pDXGIFactory)
		:m_hInstance(instance),
		m_Device(device),
		m_pDXGIFactory(pDXGIFactory)
	{

		m_bIsFullScreen = false;
		mIsSwapChain = true;//(deviceIfSwapChain != NULL);
		mIsExternal = false;
		mHwnd = 0;


		mSizing = false;
		mClosed = false;
		mHidden = false;
		mSwitchingFullscreen = false;
		mDisplayFrequency = 0;
		mRenderTargetView = 0;
		mDepthStencilView = 0;
		mpBackBuffer = 0;
	}
	///--------------------------------------------------------------------------------
	YumeD3D11RenderWindow::~YumeD3D11RenderWindow()
	{
		SAFE_RELEASE(mRenderTargetView);
		SAFE_RELEASE(mDepthStencilView);

		SAFE_RELEASE(mpBackBuffer);

		Destroy();
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Create(const YumeString& Name,
		unsigned int Width,
		unsigned int Height,
		bool FullScreen,
		const StrKeyValuePair* Params)
	{
		HINSTANCE hInst = m_hInstance;
		mVSync = false;
		mVSyncInterval = 1;
		YumeString title = Name;
		unsigned int colourDepth = 32;
		int left = -1; // Defaults to screen center
		int top = -1; // Defaults to screen center

		if (mHwnd)
		{
			Destroy();
		}

		if (!mIsExternal)
		{
			DWORD dwStyle = (mHidden ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN;
			RECT rc;

			m_uiWidth = Width;
			m_uiHeight = Height;
			m_iTop = top;
			m_iLeft = left;

			if (!m_bIsFullScreen)
			{
				/*if (parentHWnd)
				{
					dwStyle |= WS_CHILD;
				}
				else*/

				//if (border == "none")
				//	dwStyle |= WS_POPUP;
				//else if (border == "fixed")
				//	dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
				//	WS_SYSMENU | WS_MINIMIZEBOX;
				//else
				dwStyle |= WS_OVERLAPPEDWINDOW;


				/*	if (!outerSize)
					{*/
					// Calculate window dimensions required
					// to get the requested client area
				SetRect(&rc, 0, 0, m_uiWidth, m_uiHeight);
				AdjustWindowRect(&rc, dwStyle, false);
				m_uiWidth = rc.right - rc.left;
				m_uiHeight = rc.bottom - rc.top;

				// Clamp width and height to the desktop dimensions
				int screenw = GetSystemMetrics(SM_CXSCREEN);
				int screenh = GetSystemMetrics(SM_CYSCREEN);
				if ((int)m_uiWidth > screenw)
					m_uiWidth = screenw;
				if ((int)m_uiHeight > screenh)
					m_uiHeight = screenh;
				if (m_iLeft < 0)
					m_iLeft = (screenw - m_uiWidth) / 2;
				if (m_iTop < 0)
					m_iTop = (screenh - m_uiHeight) / 2;
				/*}*/
			}
			else
			{
				dwStyle |= WS_POPUP;
				m_iTop = m_iLeft = 0;
			}
			WNDCLASS wc = { 0, YumeWindowEvents::_WndProc, 0, 0, hInst,
				LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "OgreD3D11Wnd" };
			RegisterClass(&wc);
			mIsExternal = false;
			mHwnd = CreateWindow("OgreD3D11Wnd", title.c_str(), dwStyle,
				m_iLeft, m_iTop, m_uiWidth, m_uiHeight, 0, 0, hInst, this);

			YumeWindowEvents::AddRenderWindow(this);
		}


		RECT rc;
		// top and left represent outer window coordinates
		GetWindowRect(mHwnd, &rc);
		m_iTop = rc.top;
		m_iLeft = rc.left;
		// width and height represent interior drawable area
		GetClientRect(mHwnd, &rc);
		m_uiWidth = rc.right;
		m_uiHeight = rc.bottom;

		m_sName = title;
		/*mDepthBufferPoolId = depthBuffer ? DepthBuffer::POOL_DEFAULT : DepthBuffer::POOL_NO_DEPTH;*/
		m_bIsFullScreen = FullScreen;
		m_uiColourDepth = colourDepth;

		YumeLogManager::Get().stream()
			<< "D3D11 : Created D3D11 Rendering Window '"
			<< m_sName << "' : " << m_uiWidth << "x" << m_uiHeight
			<< ", " << m_uiColourDepth << "bpp";

		CreateDirect3D11Resources();

		m_bActive = true;
		mClosed = false;
		SetHidden(mHidden);
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::CreateDirect3D11Resources()
	{


	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::OnResize(unsigned int width, unsigned int height)
	{

	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Resize(unsigned int Width,
		unsigned int Height)
	{
		if (mHwnd && !m_bIsFullScreen)
		{
			RECT rc = { 0, 0, Width, Height };
			AdjustWindowRect(&rc, GetWindowLong(mHwnd, GWL_STYLE), false);
			Width = rc.right - rc.left;
			Height = rc.bottom - rc.top;
			SetWindowPos(mHwnd, 0, 0, 0, Width, Height,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Move(int left, int top)
	{
		if (mHwnd && !m_bIsFullScreen)
		{
			SetWindowPos(mHwnd, 0, top, left, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
	///--------------------------------------------------------------------------------
	bool YumeD3D11RenderWindow::IsPrimary() const
	{
		return m_bIsPrimary;
	}
	///--------------------------------------------------------------------------------
	bool YumeD3D11RenderWindow::IsFullScreen() const
	{
		return m_bIsFullScreen;
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::GetProperties(unsigned int& width, unsigned int& height, unsigned int& colourDepth,
		int& left, int& top)
	{

	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Destroy()
	{
		if (mHwnd)
		{
			DestroyWindow(mHwnd);
		}
		mHwnd = 0;
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::SetHidden(bool b)
	{

		mHidden = b;
		if (!mIsExternal)
		{
			if (b)
				ShowWindow(mHwnd, SW_HIDE);
			else
				ShowWindow(mHwnd, SW_SHOWNORMAL);
		}
	}
	///--------------------------------------------------------------------------------
	bool YumeD3D11RenderWindow::IsHidden()
	{
		return mHidden;
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::GetCustomAttribute(const YumeString& name, void* pData)
	{
		// Valid attributes and their equvalent native functions:
		// D3DDEVICE			: getD3DDevice
		// WINDOW				: getWindowHandle

		if (name == "D3DDEVICE")
		{
			ID3D11Device  **device = (ID3D11Device **)pData;
			*device = m_Device.Get();
			return;
		}
		else if (name == "WINDOW")
		{
			HWND *pHwnd = (HWND*)pData;
			*pHwnd = mHwnd;
			return;
		}
		else if (name == "isTexture")
		{
			bool *b = reinterpret_cast<bool *>(pData);
			*b = false;

			return;
		}
		else if (name == "ID3D11RenderTargetView")
		{
			*static_cast<ID3D11RenderTargetView***>(pData) = &mRenderTargetView;
			return;
		}
		else if (name == "ID3D11Texture2D")
		{
			ID3D11Texture2D **pBackBuffer = (ID3D11Texture2D**)pData;
			*pBackBuffer = mpBackBuffer;
		}
		else if (name == "numberOfViews")
		{
			unsigned int* n = reinterpret_cast<unsigned int*>(pData);
			*n = 1;
		}

	}
	///--------------------------------------------------------------------------------

}

///--------------------------------------------------------------------------------

