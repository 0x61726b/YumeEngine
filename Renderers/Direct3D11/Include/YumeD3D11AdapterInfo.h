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
#ifndef __YumeD3D11AdapterInfo_h__
#define __YumeD3D11AdapterInfo_h__
//---------------------------------------------------------------------------------
#include "YumeD3D11Required.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	static unsigned int modeCount = 0;

	class YumeD3D11AdapterInfo
	{
	private:
		DXGI_OUTPUT_DESC mDisplayMode;
		DXGI_MODE_DESC mModeDesc;
		unsigned int modeNumber;

	public:
		YumeD3D11AdapterInfo();
		YumeD3D11AdapterInfo(const YumeD3D11AdapterInfo &ob);
		YumeD3D11AdapterInfo(DXGI_OUTPUT_DESC d3ddm, DXGI_MODE_DESC ModeDesc);
		~YumeD3D11AdapterInfo();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		DXGI_FORMAT GetFormat() const;
		DXGI_RATIONAL GetRefreshRate() const;
		unsigned int GetColourDepth() const;
		DXGI_OUTPUT_DESC GetDisplayMode() const;
		DXGI_MODE_DESC GetModeDesc() const;
		void IncreaseRefreshRate(DXGI_RATIONAL rr);
		YumeString GetDescription() const;
	};
}
#endif
//---------------------------------------------------------------------------------