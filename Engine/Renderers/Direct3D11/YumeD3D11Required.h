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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeD3D11Required_h__
#define __YumeD3D11Required_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#undef NOMINMAX
#define NOMINMAX 

#include <D3D11.h>
#include <DXGI.h>

#include <SDL.h>

static const D3D11_FILTER d3dFilterMode[] =
{
	D3D11_FILTER_MIN_MAG_MIP_POINT,
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	D3D11_FILTER_ANISOTROPIC,
	D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
	D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
	D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
	D3D11_FILTER_COMPARISON_ANISOTROPIC
};

static const D3D11_TEXTURE_ADDRESS_MODE d3dAddressMode[] =
{
	D3D11_TEXTURE_ADDRESS_WRAP,
	D3D11_TEXTURE_ADDRESS_MIRROR,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_BORDER
};

#define D3D_SAFE_RELEASE(p) if (p) { ((IUnknown*)p)->Release();  p = 0; }
//----------------------------------------------------------------------------

#	if YUME_PLATFORM == YUME_PLATFORM_WIN32
#		if defined(BUILDING_YUME_DIRECT3D11)
#			define YumeD3DExport __declspec( dllexport )
#		else
#			define YumeD3DExport __declspec( dllimport )
#		endif
#	else
#	define YumeD3DExport	
#	endif


//----------------------------------------------------------------------------
#endif
