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
/// File : XX.h
/// Date : XX.XX.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

#include "YumeD3D11RenderWindow.h"
#include "YumeD3D11Renderer.h"
#include "YumeD3D11Adapter.h"
#include "Core/YumeWindowMessageListener.h"

#include "Logging/logging.h"
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
		m_pRenderTargetView = 0;
		m_pDepthStencilView = 0;
		m_pBackBuffer = 0;

		mFSAAType.Count = 1;
		mFSAAType.Quality = 0;
	}
	///--------------------------------------------------------------------------------
	YumeD3D11RenderWindow::~YumeD3D11RenderWindow()
	{
		SAFE_RELEASE(m_pRenderTargetView);
		SAFE_RELEASE(m_pDepthStencilView);

		SAFE_RELEASE(m_pBackBuffer);

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
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, L"YumeD3D11Wnd" };
			RegisterClass(&wc);
			mIsExternal = false;
			mHwnd = CreateWindowA("YumeD3D11Wnd", title.c_str(), dwStyle,
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

		YUMELOG_DEBUG(
			"D3D11 : Created D3D11 Rendering Window '"
			<< m_sName << "' : " << m_uiWidth << "x" << m_uiHeight
			<< ", " << m_uiColourDepth << "bpp");

		CreateDirect3D11Resources();

		m_bActive = true;
		mClosed = false;
		this->SetActive(m_bActive);
		SetHidden(mHidden);
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::CreateDirect3D11Resources()
	{
		if (mIsSwapChain && m_Device.isNull())
		{
			//Nope
		}

		ZeroMemory(&m_pDXGISwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		m_pDXGISwapChainDesc.Windowed = !m_bIsFullScreen;
		m_pDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		// triple buffer if VSync is on
		m_pDXGISwapChainDesc.BufferCount = mVSync ? 2 : 1;
		m_pDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_pDXGISwapChainDesc.OutputWindow = mHwnd;
		m_pDXGISwapChainDesc.BufferDesc.Width = m_uiWidth;
		m_pDXGISwapChainDesc.BufferDesc.Height = m_uiHeight;
		m_pDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		m_pDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

		m_pDXGISwapChainDesc.SampleDesc.Count = mFSAAType.Count;
		m_pDXGISwapChainDesc.SampleDesc.Quality = mFSAAType.Quality;

		if (m_bIsFullScreen)
		{
			m_pDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			m_pDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			m_pDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		}

		m_pDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//Do something with FSAA settings here

		//MSAA 

		if (mIsSwapChain)
		{

			HRESULT hr;
			IDXGIDevice1* pDXGIDevice = NULL;
			hr = m_Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGIDevice);

			if (FAILED(hr))
			{
				YUMELOG_DEBUG("Can't get DXGI device from device!");
			}

			hr = m_pDXGIFactory->CreateSwapChain(pDXGIDevice, &m_pDXGISwapChainDesc, &m_pSwapChain);

			if (FAILED(hr))
			{
				//Try again
				hr = m_pDXGIFactory->CreateSwapChain(pDXGIDevice, &m_pDXGISwapChainDesc, &m_pSwapChain);
			}

			SAFE_RELEASE(pDXGIDevice);

			if (FAILED(hr))
			{
				YUMELOG_DEBUG("Unable to create Swap Chain!");
			}

			hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pBackBuffer);

			if (FAILED(hr))
			{
				YUMELOG_DEBUG("Unable to create the back buffer from swap chain!");
			}

			D3D11_TEXTURE2D_DESC backBufferDesc;
			m_pBackBuffer->GetDesc(&backBufferDesc);

			D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
			ZeroMemory(&RTVDesc, sizeof(RTVDesc));

			RTVDesc.Format = backBufferDesc.Format;
			RTVDesc.ViewDimension = false ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			RTVDesc.Texture2D.MipSlice = 0;
			hr = m_Device->CreateRenderTargetView(m_pBackBuffer, &RTVDesc, &m_pRenderTargetView);

			if (FAILED(hr))
			{
				YumeString errorDescription = m_Device.getErrorDescription();
				YUMELOG_DEBUG("Unable to create a render target view " + errorDescription);
			}


			//Depth Stencil Buffer

			ID3D11Texture2D* pDepthStencil = NULL;
			D3D11_TEXTURE2D_DESC descDepth;

			descDepth.Width = m_uiWidth;
			descDepth.Height = m_uiHeight;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
			descDepth.SampleDesc.Count = mFSAAType.Count;
			descDepth.SampleDesc.Quality = mFSAAType.Quality;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;

			hr = m_Device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
			if (FAILED(hr) || m_Device.isError())
			{
				YumeString errorDescription = m_Device.getErrorDescription(hr);
				YUMELOG_DEBUG("Unable to create a depth texture" + errorDescription);
			}
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

			descDSV.Format = DXGI_FORMAT_D32_FLOAT;
			descDSV.ViewDimension = false ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;
			hr = m_Device->CreateDepthStencilView(pDepthStencil, &descDSV, &m_pDepthStencilView);

			SAFE_RELEASE(pDepthStencil);

			if (FAILED(hr))
			{
				YumeString errorDescription = m_Device.getErrorDescription();
				YUMELOG_DEBUG("Unable to create depth stencil view! " + errorDescription);
			}

		}

	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::DestroyDirect3D11Resources()
	{
		if (mIsSwapChain)
		{
			SAFE_RELEASE(m_pSwapChain);
		}
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Onresize(unsigned int width, unsigned int height)
	{

	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::resize(unsigned int Width,
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
	void YumeD3D11RenderWindow::Present(bool bVsync)
	{
		if (!m_Device.isNull())
		{
			HRESULT hr;
			if (mIsSwapChain)
			{
				hr = m_pSwapChain->Present(bVsync ? mVSyncInterval : 0, 0);
			}
		}
	}
	///--------------------------------------------------------------------------------
	void YumeD3D11RenderWindow::Destroy()
	{
		DestroyDirect3D11Resources();
		if (mHwnd)
		{
			YumeWindowEvents::RemoveRenderWindow(this);
			DestroyWindow(mHwnd);
		}
		mHwnd = 0;
		m_bActive = false;
		
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
			*static_cast<ID3D11RenderTargetView***>(pData) = &m_pRenderTargetView;
			return;
		}
		else if (name == "ID3D11Texture2D")
		{
			ID3D11Texture2D **pBackBuffer = (ID3D11Texture2D**)pData;
			*pBackBuffer = m_pBackBuffer;
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

