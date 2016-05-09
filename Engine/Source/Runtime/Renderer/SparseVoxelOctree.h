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
#ifndef __SparseVoxelOctree_h__
#define __SparseVoxelOctree_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeTexture3D.h"
#include "GlobalIlluminationVolume.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class RenderCall;

	class YumeAPIExport SparseVoxelOctree : public GIVolume
	{
	public:
		SparseVoxelOctree();
		virtual ~SparseVoxelOctree();

		virtual void Voxelize(RenderCall* call,YumeGeometry* geo,bool clear);
		virtual void Inject();

		virtual void Create(unsigned volumeSize);
		virtual void Filter();

		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& model, const DirectX::XMFLOAT3& svo_min, const DirectX::XMFLOAT3& svo_max);

		virtual YumeGeometry* GetVolumeGeometry() { return 0; }

		void ClearPs();
	private:
		TexturePtr vNormal;
		TexturePtr vRho;

		int volume_size_;
		UINT last_bound_;
		DirectX::XMFLOAT4X4 worldToSvo_;
		DirectX::XMFLOAT3			svo_min_, svo_max_;
	};
}


//----------------------------------------------------------------------------
#endif
