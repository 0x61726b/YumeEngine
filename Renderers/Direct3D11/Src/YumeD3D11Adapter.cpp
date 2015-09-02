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
/// File : YumeD3D11AdapterInfo.cpp
/// Date : 9.2.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

#include "YumeD3D11Adapter.h"
#include "YumeD3D11AdapterInfoList.h"
#include "YumeD3D11AdapterInfo.h"
#include "YumeD3D11Device.h"
#include "YumeString.h"

namespace YumeEngine
{
	//---------------------------------------------------------------------
	unsigned int YumeD3D11Adapter::driverCount = 0;
	//---------------------------------------------------------------------
	YumeD3D11Adapter::YumeD3D11Adapter()
	{
		tempNo = ++driverCount;
		ZeroMemory(&mAdapterIdentifier, sizeof(mAdapterIdentifier));
		ZeroMemory(&mDesktopDisplayMode, sizeof(mDesktopDisplayMode));
		mVideoModeList = NULL;
		mDXGIAdapter = NULL;
	}
	//---------------------------------------------------------------------
	YumeD3D11Adapter::YumeD3D11Adapter(const YumeD3D11Adapter &ob)
	{
		tempNo = ++driverCount;
		mAdapterNumber = ob.mAdapterNumber;
		mAdapterIdentifier = ob.mAdapterIdentifier;
		mDesktopDisplayMode = ob.mDesktopDisplayMode;
		mVideoModeList = NULL;
		mDXGIAdapter = ob.mDXGIAdapter;

	}
	//---------------------------------------------------------------------
	YumeD3D11Adapter::YumeD3D11Adapter(unsigned int adapterNumber, IDXGIAdapter1* pDXGIAdapter)
	{
		tempNo = ++driverCount;
		mAdapterNumber = adapterNumber;
		mVideoModeList = NULL;
		mDXGIAdapter = pDXGIAdapter;

		// get the description of the adapter
		pDXGIAdapter->GetDesc1(&mAdapterIdentifier);

	}
	//---------------------------------------------------------------------
	YumeD3D11Adapter::~YumeD3D11Adapter()
	{
		SAFE_DELETE(mVideoModeList);
		driverCount--;
	}
	//---------------------------------------------------------------------
	YumeString YumeD3D11Adapter::DriverName() const
	{
		size_t size = wcslen(mAdapterIdentifier.Description);
		char * str = new char[size + 1];

		wcstombs(str, mAdapterIdentifier.Description, size);
		str[size] = '\0';
		YumeString Description = str;
		delete str;
		return YumeString(Description);
	}
	//---------------------------------------------------------------------
	YumeString YumeD3D11Adapter::DriverDescription() const
	{
		size_t size = wcslen(mAdapterIdentifier.Description);
		char * str = new char[size + 1];

		wcstombs(str, mAdapterIdentifier.Description, size);
		str[size] = '\0';
		YumeString driverDescription = str;
		delete[] str;
		StringUtil::trim(driverDescription);

		return  driverDescription;
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfoList* YumeD3D11Adapter::getVideoModeList()
	{
		if (!mVideoModeList)
			mVideoModeList = new YumeD3D11AdapterInfoList(this);

		return mVideoModeList;
	}
	//---------------------------------------------------------------------
	unsigned int YumeD3D11Adapter::getAdapterNumber() const
	{
		return mAdapterNumber;
	}
	//---------------------------------------------------------------------
	const DXGI_ADAPTER_DESC1& YumeD3D11Adapter::getAdapterIdentifier() const
	{
		return mAdapterIdentifier;
	}
	//---------------------------------------------------------------------
	const DXGI_MODE_DESC& YumeD3D11Adapter::getDesktopMode() const
	{
		return mDesktopDisplayMode;
	}
	//---------------------------------------------------------------------
	IDXGIAdapter1* YumeD3D11Adapter::getDeviceAdapter() const
	{
		return mDXGIAdapter;
	}
	//---------------------------------------------------------------------
}