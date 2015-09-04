#pragma once
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
/// File : YumeRenderTarget.h
/// Date : 8.31.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeRenderTarget_h__
#define __YumeRenderTarget_h__
//---------------------------------------------------------------------------------
#include "YumeRequired.h"

//---------------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport YumeRenderTarget : public RenderObjAlloc
	{
	public:
		YumeRenderTarget();
		virtual ~YumeRenderTarget();

		virtual const YumeString& GetName() const;

		virtual unsigned int GetWidth() const;
		virtual unsigned int GetHeight() const;
		virtual unsigned int GetColourDepth() const;

		virtual void Update();

		virtual void Present(bool vSync = true);


		virtual void SetPriority(unsigned char priority) { m_ucPriority = priority; }
		
		virtual unsigned char GetPriority() const { return m_ucPriority; }

		virtual void GetCustomAttribute(const YumeString& name, void* pData);

		virtual bool IsActive() const;

		void SetActive(bool b);
	protected:
		YumeString						m_sName;
		unsigned int					m_uiWidth;
		unsigned int					m_uiHeight;
		unsigned int					m_uiColourDepth;
		unsigned char					m_ucPriority;
		bool							m_bActive;
	};
}
//---------------------------------------------------------------------------------
#endif
//~End of YumeRenderer.h