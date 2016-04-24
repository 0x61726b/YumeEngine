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
#ifndef __Light_h__
#define __Light_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "SceneNode.h"
#include "Math/YumeColor.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	enum LightType
	{
		LT_DIRECTIONAL = 0
	};

	class YumeAPIExport Light : public SceneNode
	{
	public:
		Light();
		virtual ~Light();

		void SetType(LightType type) {type_ = type;}
		void SetColor(const YumeColor& color) { color_ = color; }

		void UpdateLightParameters();


		LightType GetType() const { return type_; }
		const YumeColor& GetColor() const { return color_; }
	private:
		LightType type_;
		YumeColor color_;
	};
}


//----------------------------------------------------------------------------
#endif
