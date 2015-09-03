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
/// File : YumeD3D11Renderer.h
/// Date : 3.9.2015
/// Comments : 
//
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumeD3D11Renderer_h__
#define __YumeD3D11Renderer_h__
//--------------------------------------------------------------------------------
#include "YumeD3D11Required.h"
#include "YumeRenderer.h"
#include "YumeD3D11Device.h"
//--------------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3D11Renderer : public YumeRenderer
	{
	private:
		enum D3D11_DRIVER_TYPE
		{
			HARDWARE,
			SOFTWARE,
			WARP
		};

		D3D11_DRIVER_TYPE m_DriverType;

		YumeD3D11Device m_Device;

		HINSTANCE m_hInstance;

		YumeD3D11AdapterList* m_AdapterList;

		YumeD3D11Adapter* m_CurrAdapter;

		YumeD3D11AdapterList* GetAdapters();

		YumeD3D11RenderWindow* m_CurrentWindow;

		typedef YumeVector<YumeD3D11RenderWindow*>::type SecondaryWindowList;
		// List of additional windows after the first (swap chains)
		SecondaryWindowList mSecondaryWindows;

		IDXGIFactory1* m_pDXGIFactory;

		ConfigOptionMap m_Options;

		void RefreshD3DSettings();

		RenderTargetMap mRenderTargets;
		RenderTargetPriorityMap mPrioritisedRenderTargets;
		YumeRenderTarget* m_pActiveRenderTarget;


		YumeRendererCapabilities* mRealCapabilities;
		YumeRendererCapabilities* mCurrentCapabilities;

		void AttachRenderTarget(YumeRenderTarget& target);
	public:
		YumeD3D11Renderer(HINSTANCE hInst);

		~YumeD3D11Renderer();

		void Init();
		void Configure();

		//Renderer Interface
		const YumeString& GetName();

		YumeRenderWindow* Initialize(bool autoCreate, const YumeString& Title = "Yume Engine");

		YumeRendererCapabilities* CreateRendererCapabilities() const;

		YumeRenderWindow* CreateRenderWindow(const YumeString &name, unsigned int width, unsigned int height,
			bool fullScreen, const StrKeyValuePair *miscParams = 0);
		//~


	private:
		bool m_bRendererInitialized;
	};
}

//--------------------------------------------------------------------------------



///--------------------------------------------------------------------------------
//End of __YumeD3D11Renderer_h__
#endif