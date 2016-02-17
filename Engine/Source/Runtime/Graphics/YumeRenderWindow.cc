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
/// File : YumeRenderWindow.cpp
/// Date : 8.31.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "Graphics/YumeRenderWindow.h"

#include "Graphics/YumeRenderer.h"

//---------------------------------------------------------------------------------

namespace YumeEngine
{
	//---------------------------------------------------------------------------------
	YumeRenderWindow::YumeRenderWindow() : YumeRenderTarget()
	{
		m_bIsPrimary = false;
		mAutoDeactivatedOnFocusChange = true;
	}
	//---------------------------------------------------------------------------------
	bool YumeRenderWindow::IsPrimary() const
	{
		return m_bIsPrimary;
	}
	//---------------------------------------------------------------------------------
	bool YumeRenderWindow::IsFullScreen() const
	{
		return m_bIsFullScreen;
	}
	//---------------------------------------------------------------------------------
	void YumeRenderWindow::GetProperties(unsigned int& width, unsigned int& height, unsigned int& colourDepth,
		int& left, int& top)
	{
		width = m_uiWidth;
		height = m_uiHeight;
		colourDepth = m_uiColourDepth;
		left = m_iLeft;
		top = m_iTop;
	}
	//---------------------------------------------------------------------------------
	void YumeRenderWindow::SetDeactivateOnFocusChange(bool b)
	{
		mAutoDeactivatedOnFocusChange = b;
	}
	//---------------------------------------------------------------------------------
	bool YumeRenderWindow::IsDeactivatedOnFocusChange() const
	{
		return mAutoDeactivatedOnFocusChange;
	}
}