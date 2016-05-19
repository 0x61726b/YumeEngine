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
#include "YumeHeaders.h"
#include "YumeUIElement.h"
#include "YumeUI.h"

#include "Renderer/YumeTexture2D.h"
#include "Renderer/YumeRHI.h"

#include <cef3d/Cef3D.h>

namespace YumeEngine
{
	YumeUIElement::YumeUIElement(int width,int height,const YumeString& name)
		: name_(name),
		url_(String::EMPTY),
		texture_(0),
		browserIndex_(0),
		visible_(false)
	{
		rect_ = IntRect(0,0,width,height);
	}

	YumeUIElement::~YumeUIElement()
	{
		bindings_.clear();
	}

	void YumeUIElement::PrepareResources()
	{
		texture_ = (gYume->pRHI->CreateTexture2D());
		texture_->SetSize(rect_.right_ - rect_.left_,rect_.bottom_ - rect_.top_,gYume->pRHI->GetBGRAFormatNs(),TEXTURE_DYNAMIC);

		browserIndex_ = gYume->pUI->CreateBrowser(this);
	}

	void YumeUIElement::Update()
	{
		if(!bindingsNeedUpdate_)
			return;

		for(int i=0; i < bindings_.size(); ++i)
		{
			YumeUIBinding* binding = bindings_[i];
			if(binding->GetDirty() && binding->GetBindingType() == BINDING_STATIC)
			{
				gYume->pUI->SendEvent("set" + binding->GetName(),"{\"" + binding->GetName() + "\": \"" + binding->GetValue().ToString() + "\"}");
				binding->ClearDirty();
			}
		}

		bindingsNeedUpdate_ = false;
	}

	void YumeUIElement::SetWidth(int width)
	{
		int oldWidth = rect_.right_ - rect_.left_;
		int right = rect_.right_;
		int left = rect_.left_;
		int top = rect_.top_;
		int bottom = rect_.bottom_;

		int newRight = left + width;
		rect_.right_ = newRight;
	}

	void YumeUIElement::SetHeight(int height)
	{
		int oldWidth = rect_.right_ - rect_.left_;
		int right = rect_.right_;
		int left = rect_.left_;
		int top = rect_.top_;
		int bottom = rect_.bottom_;

		int newBottom = top + height;
		rect_.bottom_ = newBottom;
	}

	void YumeUIElement::SetPosition(int x,int y)
	{
		rect_.left_ = x;
		rect_.top_ = y;
	}

	void YumeUIElement::MarkForUpdate()
	{
		bindingsNeedUpdate_ = true;
	}
	void YumeUIElement::ClearUpdate()
	{
		bindingsNeedUpdate_ = false;
	}


	void YumeUIElement::AddBinding(YumeUIBinding* binding)
	{
		bindings_.push_back(binding);
	}


	void YumeUIElement::SetRect(const IntRect& rect)
	{
		rect_ = rect;
	}


	void YumeUIElement::SetURL(const YumeString& url)
	{
		url_ = url;
	}

	void YumeUIElement::SetVisible(bool enable)
	{
		visible_ = enable;
	}

	YumeUIBinding* YumeUIElement::GetBinding(const YumeString& name)
	{
		for(int i=0; i < bindings_.size(); ++i)
		{
			if(bindings_[i]->GetName() == name)
				return bindings_[i];
		}
		return 0;
	}
}
