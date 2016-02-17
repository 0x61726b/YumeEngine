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
/// File : YumeD3D11AdapterInfo.cpp
/// Date : 9.2.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////
#include "YumeD3D11AdapterInfo.h"

//---------------------------------------------------------------------------------
namespace YumeEngine
{
	//---------------------------------------------------------------------
	YumeString YumeD3D11AdapterInfo::GetDescription() const
	{
		char tmp[128];
		unsigned int colourDepth = 16;
		if (mModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_TYPELESS ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_UINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_SINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_TYPELESS ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_FLOAT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_UINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_SINT
			)
			colourDepth = 32;

		sprintf(tmp, "%d x %d @ %d-bit colour", mModeDesc.Width, mModeDesc.Height, colourDepth);
		return YumeString(tmp);
	}
	//---------------------------------------------------------------------
	unsigned int YumeD3D11AdapterInfo::GetColourDepth() const
	{
		unsigned int colourDepth = 16;
		if (mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_TYPELESS ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_UINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_SINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_TYPELESS ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_FLOAT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_UINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_SINT
			)
			colourDepth = 32;

		return colourDepth;
	}
	//---------------------------------------------------------------------
	unsigned int YumeD3D11AdapterInfo::GetWidth() const
	{
		return mModeDesc.Width;
	}
	//---------------------------------------------------------------------
	unsigned int YumeD3D11AdapterInfo::GetHeight() const
	{
		return mModeDesc.Height;
	}
	//---------------------------------------------------------------------
	DXGI_FORMAT YumeD3D11AdapterInfo::GetFormat() const
	{
		return mModeDesc.Format;
	}
	//---------------------------------------------------------------------
	DXGI_RATIONAL YumeD3D11AdapterInfo::GetRefreshRate() const
	{
		return mModeDesc.RefreshRate;
	}
	//---------------------------------------------------------------------
	DXGI_OUTPUT_DESC YumeD3D11AdapterInfo::GetDisplayMode() const
	{
		return mDisplayMode;
	}
	//---------------------------------------------------------------------
	DXGI_MODE_DESC YumeD3D11AdapterInfo::GetModeDesc() const
	{
		return mModeDesc;
	}
	//---------------------------------------------------------------------
	void YumeD3D11AdapterInfo::IncreaseRefreshRate(DXGI_RATIONAL rr)
	{
		mModeDesc.RefreshRate = rr;
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfo::YumeD3D11AdapterInfo(DXGI_OUTPUT_DESC d3ddm, DXGI_MODE_DESC ModeDesc)
	{
		modeNumber = ++modeCount; mDisplayMode = d3ddm; mModeDesc = ModeDesc;
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfo::YumeD3D11AdapterInfo(const YumeD3D11AdapterInfo &ob)
	{
		modeNumber = ++modeCount; mDisplayMode = ob.mDisplayMode; mModeDesc = ob.mModeDesc;
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfo::YumeD3D11AdapterInfo()
	{
		modeNumber = ++modeCount; ZeroMemory(&mDisplayMode, sizeof(DXGI_OUTPUT_DESC)); ZeroMemory(&mModeDesc, sizeof(DXGI_MODE_DESC));
	}
	//---------------------------------------------------------------------
	YumeD3D11AdapterInfo::~YumeD3D11AdapterInfo()
	{
		modeCount--;
	}
	//---------------------------------------------------------------------
}
//---------------------------------------------------------------------------------