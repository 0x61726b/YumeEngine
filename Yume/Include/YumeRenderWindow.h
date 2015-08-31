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
/// File : YumeRenderWindow.h
/// Date : 8.31.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef __YumeRenderWindow_h__
#define __YumeRenderWindow_h__
//---------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeRenderTarget.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport YumeRenderWindow : public YumeRenderTarget
	{
	public:
		YumeRenderWindow();

		virtual void Create(const YumeString& Name,
			unsigned int Width,
			unsigned int Height,
			bool FullScreen,
			const StrKeyValuePair* Params) = 0;

		virtual void Destroy() = 0;
		
		virtual void Resize(unsigned int Width,
							unsigned int Height) = 0;

		virtual void Move(int left, int top) = 0;

		virtual void GetProperties(unsigned int& width, unsigned int& height, unsigned int& colourDepth,
			int& left, int& top);

		virtual bool IsPrimary() const;

		virtual bool IsFullScreen() const;

	protected:
		int m_iLeft;
		int m_iTop;

		bool m_bIsFullScreen;
		bool m_bIsPrimary;

	};
}
//---------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------
//~End of __YumeRenderWindow_h__