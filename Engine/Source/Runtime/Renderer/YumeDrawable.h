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
#ifndef __YumeDrawable_h__
#define __YumeDrawable_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeRendererDefs.h"
#include "Math/YumeBoundingBox.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	static const unsigned DRAWABLE_GEOMETRY = 0x1;
	static const unsigned DRAWABLE_LIGHT = 0x2;
	static const unsigned DRAWABLE_ZONE = 0x4;
	static const unsigned DRAWABLE_GEOMETRY2D = 0x8;
	static const unsigned DRAWABLE_ANY = 0xff;
	static const unsigned DEFAULT_VIEWMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_LIGHTMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_SHADOWMASK = M_MAX_UNSIGNED;
	static const unsigned DEFAULT_ZONEMASK = M_MAX_UNSIGNED;
	static const int MAX_VERTEX_LIGHTS = 4;
	static const float ANIMATION_LOD_BASESCALE = 2500.0f;


}


//----------------------------------------------------------------------------
#endif
