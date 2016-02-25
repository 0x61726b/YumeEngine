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
#include "YumeRect.h"



namespace YumeEngine
{

	const Rect Rect::FULL(-1.0f,-1.0f,1.0f,1.0f);
	const Rect Rect::POSITIVE(0.0f,0.0f,1.0f,1.0f);
	const Rect Rect::ZERO(0.0f,0.0f,0.0f,0.0f);

	const IntRect IntRect::ZERO(0,0,0,0);

	YumeString Rect::ToString() const
	{
		char tempBuffer[128];
		sprintf(tempBuffer,"%g %g %g %g",min_.x,min_.y,max_.x,max_.y);
		return YumeString (tempBuffer);
	}

	YumeString  IntRect::ToString() const
	{
		char tempBuffer[128];
		sprintf(tempBuffer,"%d %d %d %d",left_,top_,right_,bottom_);
		return YumeString (tempBuffer);
	}

	void Rect::Clip(const Rect& rect)
	{
		if(rect.min_.x > min_.x)
			min_.x = rect.min_.x;
		if(rect.max_.x < max_.x)
			max_.x = rect.max_.x;
		if(rect.min_.y > min_.y)
			min_.y = rect.min_.y;
		if(rect.max_.y < max_.y)
			max_.y = rect.max_.y;

		if(min_.x > max_.x || min_.y > max_.y)
		{
			min_ = Vector2(Math::POS_INFINITY,Math::POS_INFINITY);
			max_ = Vector2(Math::NEG_INFINITY,Math::NEG_INFINITY);
		}
	}
}
