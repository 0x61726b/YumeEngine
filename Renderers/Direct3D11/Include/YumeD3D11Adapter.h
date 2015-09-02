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
/// File : YumeD3D11AdapterInfo.h
/// Date : 9.2.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeD3D11Adapter_h__
#define __YumeD3D11Adapter_h__
//---------------------------------------------------------------------------------
#include "YumeD3D11Required.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3D11AdapterInfo;
	class YumeD3D11AdapterInfoList;

	class YumeD3D11Adapter
	{
	private:
		// D3D only allows one device per adapter, so it can safely be stored
		// here as well.
		unsigned int mAdapterNumber;
		DXGI_ADAPTER_DESC1 mAdapterIdentifier;
		DXGI_MODE_DESC mDesktopDisplayMode;
		YumeD3D11AdapterInfoList* mVideoModeList;
		unsigned int tempNo;
		static unsigned int driverCount;
		IDXGIAdapter1*	mDXGIAdapter;


	public:
		// Constructors
		YumeD3D11Adapter();		// Default
		YumeD3D11Adapter(const YumeD3D11Adapter &ob);	// Copy
		YumeD3D11Adapter(unsigned int adapterNumber, IDXGIAdapter1* pDXGIAdapter);
		~YumeD3D11Adapter();

		// Information accessors
		YumeString DriverName() const;
		YumeString DriverDescription() const;

		// change the device
		unsigned int getAdapterNumber() const;
		const DXGI_ADAPTER_DESC1& getAdapterIdentifier() const;
		const DXGI_MODE_DESC& getDesktopMode() const;
		IDXGIAdapter1* getDeviceAdapter() const;
		YumeD3D11AdapterInfoList* getVideoModeList();
	};
}

//---------------------------------------------------------------------------------
#endif