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
#ifndef __GlobalIlluminationVolume_h__
#define __GlobalIlluminationVolume_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include <DirectXMath.h>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGeometry;

	class YumeAPIExport GIVolume : public YumeBase
	{
	public:
		GIVolume();
		~GIVolume();

		virtual void Create(unsigned volumeSize) = 0;
		virtual void Inject() = 0;

		virtual void Voxelize(RenderCall* call,YumeGeometry* geo,bool clear) { } //SVO Only
		virtual void Filter() { } //SVO Only

		virtual YumeGeometry* GetVolumeGeometry() = 0;

		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& model,const DirectX::XMFLOAT3& lpvMin,const DirectX::XMFLOAT3& lpvMax) = 0;
	};
}


//----------------------------------------------------------------------------
#endif
