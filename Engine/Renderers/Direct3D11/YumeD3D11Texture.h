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
#ifndef __YumeD3D11Texture_h__
#define __YumeD3D11Texture_h__
//----------------------------------------------------------------------------
#include "YumeD3D11Required.h"
#include "Renderer/YumeTexture.h"
#include "YumeD3D11GpuResource.h"

//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeRHI;


	class YumeD3DExport YumeD3D11Texture : public YumeTexture,public YumeD3D11Resource
	{
	public:
		YumeD3D11Texture(YumeRHI*);

		virtual ~YumeD3D11Texture();

		unsigned GetRowDataSize(int width) const;

		void UpdateParameters();

		void CheckTextureBudget(YumeHash type);
	};
}


//----------------------------------------------------------------------------
#endif
