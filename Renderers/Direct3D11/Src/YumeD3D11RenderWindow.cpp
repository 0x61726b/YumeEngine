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

	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Resize(unsigned int Width,
		unsigned int Height)
	{

	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Move(int left, int top)
	{

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
			bool *b = reinterpret_cast< bool * >(pData);
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

