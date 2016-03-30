//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeUIElement_h__
#define __YumeUIElement_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeUIBinding.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeTexture2D;

	class YumeAPIExport YumeUIElement :
		public YumeBase
	{
	public:
		YumeUIElement(const YumeString& name = "UIElement");
		virtual ~YumeUIElement();

		void AddBinding(YumeUIBinding* binding);
		void SetRect(const IntRect& rect);
		void SetURL(const YumeString& string);
		void SetVisible(bool enable);

		virtual void OnContextReady() { }
		virtual void Update();

		void PrepareResources();

		void MarkForUpdate();
		void ClearUpdate();

		YumeUIBinding* GetBinding(const YumeString& );
		
		
		typedef YumeVector<YumeUIBinding*> UIBindings;

		UIBindings::type& GetBindings() { return bindings_; }
		const IntRect& GetRect() const { return rect_; }
		YumeTexture2D* GetTexture() const { return texture_; }
		const YumeString& GetURL() const { return url_; }
		const YumeString& GetName() const { return name_; }
		unsigned GetBrowserIndex() const { return browserIndex_; }
		bool GetVisible() const {return visible_; }

	protected:
		YumeTexture2D* texture_;
		IntRect rect_;
		UIBindings::type bindings_;
		YumeString name_;
		YumeString url_;
		unsigned browserIndex_;
		bool visible_;
		bool bindingsNeedUpdate_;
	};
}


//----------------------------------------------------------------------------
#endif
