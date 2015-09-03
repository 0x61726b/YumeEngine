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
/// File : YumeD3D11AdapterList.h
/// Date : 3.9.2015
/// Comments : 
//
///////////////////////////////////////////////////////////////////////////////////
#include "YumeD3D11AdapterList.h"
#include "YumeLogManager.h"
#include "YumeD3D11Device.h"
#include "YumeD3D11Adapter.h"
///--------------------------------------------------------------------------------
namespace YumeEngine
{
	//-----------------------------------------------------------------------
	YumeD3D11AdapterList::YumeD3D11AdapterList(IDXGIFactory1*	pDXGIFactory)
	{
		Enumerate(pDXGIFactory);
	}
	//-----------------------------------------------------------------------
	YumeD3D11AdapterList::~YumeD3D11AdapterList(void)
	{
		for (int i = 0; i<0; i++)
		{
			delete (mDriverList[i]);
		}
		mDriverList.clear();

	}
	//-----------------------------------------------------------------------
	BOOL YumeD3D11AdapterList::Enumerate(IDXGIFactory1*	pDXGIFactory)
	{
		YumeLogManager::Get().logMessage("D3D11: Driver Detection Starts");
		// Create the DXGI Factory

		for (UINT iAdapter = 0; ; iAdapter++)
		{
			IDXGIAdapter1*					pDXGIAdapter;
			HRESULT hr = pDXGIFactory->EnumAdapters1(iAdapter, &pDXGIAdapter);
			if (DXGI_ERROR_NOT_FOUND == hr)
			{
				hr = S_OK;
				break;
			}
			if (FAILED(hr))
			{
				delete pDXGIAdapter;
				return false;
			}

			// we don't want NVIDIA PerfHUD in the list - so - here we filter it out
			DXGI_ADAPTER_DESC1 adaptDesc;
			if (SUCCEEDED(pDXGIAdapter->GetDesc1(&adaptDesc)))
			{
				const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;

				if (isPerfHUD)
				{
					continue;
				}
			}

			mDriverList.push_back(new YumeD3D11Adapter(iAdapter, pDXGIAdapter));

		}

		YumeLogManager::Get().logMessage("D3D11: Driver Detection Ends");

		return TRUE;
	}
	//-----------------------------------------------------------------------
	size_t YumeD3D11AdapterList::GetCount() const
	{
		return mDriverList.size();
	}
	//-----------------------------------------------------------------------
	YumeD3D11Adapter* YumeD3D11AdapterList::Get(size_t index)
	{
		return mDriverList.at(index);
	}
	//-----------------------------------------------------------------------
	YumeD3D11Adapter* YumeD3D11AdapterList::Get(const YumeString &name)
	{
		YumeVector<YumeD3D11Adapter*>::type::iterator it = mDriverList.begin();
		if (it == mDriverList.end())
			return NULL;

		for (; it != mDriverList.end(); ++it)
		{
			if ((*it)->DriverDescription() == name)
				return (*it);
		}

		return NULL;
	}
	//-----------------------------------------------------------------------
}
