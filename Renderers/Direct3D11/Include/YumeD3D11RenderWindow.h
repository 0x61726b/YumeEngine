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
/// File : YumeD3D11RenderWindow.h
/// Date : 3.9.2015
/// Comments : 
///--------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////

#ifndef __YumeD3D11RenderWindow_h__
#define __YumeD3D11RenderWindow_h__

#include "YumeD3D11Required.h"
#include "YumeRenderWindow.h"

namespace YumeEngine
{
	class YumeD3D11RenderWindow : public YumeRenderWindow
	{
	public:
		YumeD3D11RenderWindow(HINSTANCE hInst, YumeD3D11Device& dev, IDXGIFactory1* pDXGIFactory);
		virtual ~YumeD3D11RenderWindow();

		void Create(const YumeString& Name,
			unsigned int Width,
			unsigned int Height,
			bool FullScreen,
			const StrKeyValuePair* Params);

		void Destroy();

		void Resize(unsigned int Width,
			unsigned int Height);

		void Move(int left, int top);

		void GetProperties(unsigned int& width, unsigned int& height, unsigned int& colourDepth,
			int& left, int& top);

		bool IsPrimary() const;

		bool IsFullScreen() const;

		void GetCustomAttribute(const YumeString& name, void* pData);

	protected:
		HINSTANCE m_hInstance;
		YumeD3D11Device& m_Device;
		IDXGIFactory1* m_pDXGIFactory;

		HWND mHwnd;
		bool	mIsExternal;			
		bool	mSizing;
		bool	mClosed;
		bool	mHidden;
		bool	mIsSwapChain;			// Is this a secondary window?
		bool	mSwitchingFullscreen;	// Are we switching from fullscreen to windowed or vice versa

										// Pointer to swap chain, only valid if mIsSwapChain
		IDXGISwapChain * mpSwapChain;
		DXGI_SWAP_CHAIN_DESC md3dpp;
		DXGI_SAMPLE_DESC mFSAAType;
		//DWORD mFSAAQuality;
		UINT mDisplayFrequency;
		bool mVSync;
		unsigned int mVSyncInterval;
		bool mUseNVPerfHUD;
		ID3D11RenderTargetView*		mRenderTargetView;
		ID3D11DepthStencilView*		mDepthStencilView;
		ID3D11Texture2D*			mpBackBuffer;
	};
}


///--------------------------------------------------------------------------------
//End of __YumeD3D11RenderWindow_h__
#endif
