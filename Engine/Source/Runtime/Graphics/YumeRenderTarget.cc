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
/// File : YumeRenderTarget.cpp
/// Date : 8.31.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeRenderTarget.h"

#include "Graphics/YumeRenderer.h"
//---------------------------------------------------------------------------------

namespace YumeEngine
{
	YumeRenderTarget::YumeRenderTarget()
	{
		m_bActive = true;
		m_bAutoUpdated = true;
	}
	//---------------------------------------------------------------------------------
	YumeRenderTarget::~YumeRenderTarget()
	{
		//Remove viewports
	}
	//---------------------------------------------------------------------------------
	const YumeString& YumeRenderTarget::GetName() const
	{
		return m_sName;
	}
	//---------------------------------------------------------------------------------
	unsigned int YumeRenderTarget::GetWidth() const
	{
		return m_uiWidth;
	}
	//---------------------------------------------------------------------------------
	unsigned int YumeRenderTarget::GetHeight() const
	{
		return m_uiHeight;
	}
	//---------------------------------------------------------------------------------
	unsigned int YumeRenderTarget::GetColourDepth() const
	{
		return m_uiColourDepth;
	}
	//---------------------------------------------------------------------------------
	bool YumeRenderTarget::IsActive() const
	{
		return m_bActive;
	}
	bool YumeRenderTarget::IsPrimary()
	{
		return false;
	}
	//---------------------------------------------------------------------------------
	void YumeRenderTarget::SetActive(bool b)
	{
		m_bActive = b;
	}
	//---------------------------------------------------------------------------------
	void YumeRenderTarget::Update(bool swap)
	{
		if (swap)
		{
			Present(true);
		}
	}
	//---------------------------------------------------------------------------------
	void YumeRenderTarget::GetCustomAttribute(const YumeString& name, void* pData)
	{
		//
	}
	//---------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------------
//~End of YumeRenderTarget.cpp