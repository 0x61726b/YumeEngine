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
#include "YumeCommon.h" //remove this in da future
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

		bool IsVisible() const
		{
			return true;
		}

		bool IsActive() const
		{
			return m_bActive && IsVisible();
		}

		
		//Events
		virtual void OnResize(unsigned int width, unsigned int height) = 0;

		/** Notify that the window has been resized
		@remarks
		You don't need to call this unless you created the window externally.
		*/
		virtual void OnWindowMovedOrResized() {}
		//~

		/** Returns true if the window will automatically de-activate itself when it loses focus.
		*/
		bool IsDeactivatedOnFocusChange() const;

		/** Indicates whether the window will automatically deactivate itself when it loses focus.
		* \param deactivate a value of 'true' will cause the window to deactivate itself when it loses focus.  'false' will allow it to continue to render even when window focus is lost.
		* \note 'true' is the default behavior.
		*/
		void SetDeactivateOnFocusChange(bool deactivate);

		virtual bool IsHidden(void) const { return false; }

		/** Hide (or show) the window. If called with hidden=true, this
		will make the window completely invisible to the user.
		@remarks
		Setting a window to hidden is useful to create a dummy primary
		RenderWindow hidden from the user so that you can create and
		recreate your actual RenderWindows without having to recreate
		all your resources.
		*/
		virtual void SetHidden(bool hidden)
		{
			(void)hidden;
		}

	protected:
		int m_iLeft;
		int m_iTop;

		bool m_bIsFullScreen;
		bool m_bIsPrimary;
		bool mAutoDeactivatedOnFocusChange;

		friend class YumeCentrum;
	};
}
//---------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------
//~End of __YumeRenderWindow_h__