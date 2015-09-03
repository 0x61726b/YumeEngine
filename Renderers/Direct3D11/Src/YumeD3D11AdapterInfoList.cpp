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

#include "YumeD3D11AdapterInfoList.h"
#include "YumeD3D11Adapter.h"
#include "YumeD3D11AdapterInfo.h"

namespace YumeEngine
{
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfoList::YumeD3D11AdapterInfoList(YumeD3D11Adapter* pDriver)
	{
		/*if (NULL == pDriver)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "pDriver parameter is NULL", "D3D11VideoModeList::D3D11VideoModeList");*/

		mDriver = pDriver;
		enumerate();
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfoList::~YumeD3D11AdapterInfoList()
	{
		mDriver = NULL;
		mModeList.clear();
	}
	//---------------------------------------------------------------------
	BOOL YumeD3D11AdapterInfoList::enumerate()
	{
		//		int pD3D = mDriver->getD3D();
		UINT adapter = mDriver->getAdapterNumber();
		HRESULT hr;
		IDXGIOutput *pOutput;
		for (int iOutput = 0; ; ++iOutput)
		{
			//AIZTODO: one output for a single monitor ,to be handled for mulimon	    
			hr = mDriver->getDeviceAdapter()->EnumOutputs(iOutput, &pOutput);
			if (DXGI_ERROR_NOT_FOUND == hr)
			{
				return false;
			}
			else if (FAILED(hr))
			{
				return false;	//Something bad happened.
			}
			else //Success!
			{
				const DXGI_FORMAT allowedAdapterFormatArray[] =
				{
					DXGI_FORMAT_R8G8B8A8_UNORM,			//This is DXUT's preferred mode

														//DXGI_FORMAT_R16G16B16A16_FLOAT,
														//DXGI_FORMAT_R10G10B10A2_UNORM,
														//DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				};
				int allowedAdapterFormatArrayCount = sizeof(allowedAdapterFormatArray) / sizeof(allowedAdapterFormatArray[0]);

				UINT NumModes = 512;
				DXGI_MODE_DESC *pDesc = new DXGI_MODE_DESC[NumModes];
				hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,//allowedAdapterFormatArray[f],
					0,
					&NumModes,
					pDesc);
				DXGI_OUTPUT_DESC OutputDesc;
				pOutput->GetDesc(&OutputDesc);
				for (UINT m = 0; m<NumModes; m++)
				{
					DXGI_MODE_DESC displayMode = pDesc[m];
					// Filter out low-resolutions
					if (displayMode.Width < 640 || displayMode.Height < 400)
						continue;

					// Check to see if it is already in the list (to filter out refresh rates)
					BOOL found = FALSE;
					YumeVector<YumeD3D11AdapterInfo>::type::iterator it;
					for (it = mModeList.begin(); it != mModeList.end(); it++)
					{
						DXGI_OUTPUT_DESC oldOutput = it->GetDisplayMode();
						DXGI_MODE_DESC oldDisp = it->GetModeDesc();
						if (//oldOutput.Monitor==OutputDesc.Monitor &&
							oldDisp.Width == displayMode.Width &&
							oldDisp.Height == displayMode.Height// &&
																//oldDisp.Format == displayMode.Format
							)
						{
							// Check refresh rate and favour higher if poss
							//if (oldDisp.RefreshRate < displayMode.RefreshRate)
							//	it->increaseRefreshRate(displayMode.RefreshRate);
							found = TRUE;
							break;
						}
					}

					if (!found)
						mModeList.push_back(YumeD3D11AdapterInfo(OutputDesc, displayMode));

				}

			}
		}
		return TRUE;
	}
	//---------------------------------------------------------------------
	size_t YumeD3D11AdapterInfoList::count()
	{
		return mModeList.size();
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfo* YumeD3D11AdapterInfoList::item(size_t index)
	{
		YumeVector<YumeD3D11AdapterInfo>::type::iterator p = mModeList.begin();

		return &p[index];
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfo* YumeD3D11AdapterInfoList::item(const YumeString &name)
	{
		YumeVector<YumeD3D11AdapterInfo>::type::iterator it = mModeList.begin();
		if (it == mModeList.end())
			return NULL;

		for (; it != mModeList.end(); ++it)
		{
			if (it->GetDescription() == name)
				return &(*it);
		}

		return NULL;
	}
	//---------------------------------------------------------------------
}