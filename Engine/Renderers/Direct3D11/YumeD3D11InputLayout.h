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
#ifndef __YumeD3D11InputLayout_h__
#define __YumeD3D11InputLayout_h__
//----------------------------------------------------------------------------
#include "YumeD3D11Required.h"
#include "Renderer/YumeInputLayout.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeD3DExport YumeD3D11InputLayout : public YumeInputLayout
	{
	public:
		YumeD3D11InputLayout(YumeRHI* graphics,YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks);

		virtual ~YumeD3D11InputLayout();


	};
}


//----------------------------------------------------------------------------
#endif
