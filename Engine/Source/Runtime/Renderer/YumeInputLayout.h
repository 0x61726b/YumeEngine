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
#ifndef __YumeInputLayout_h__
#define __YumeInputLayout_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeRHI;
	class YumeShaderVariation;
	class YumeVertexBuffer;

	class YumeAPIExport YumeInputLayout
	{
	public:
		YumeInputLayout(YumeRHI* graphics,YumeShaderVariation* vertexShader,YumeVertexBuffer** buffers,unsigned* elementMasks) { };
		virtual ~YumeInputLayout() { };

		
		void* GetInputLayout() const { return inputLayout_; }

	protected:
		/// Input layout object.
		void* inputLayout_;
	};
}


//----------------------------------------------------------------------------
#endif
