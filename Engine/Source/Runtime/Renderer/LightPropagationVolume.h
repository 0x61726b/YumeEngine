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
#ifndef __YumeLPV_h__
#define __YumeLPV_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include <DirectXMath.h>
#include "YumeGeometry.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeTexture2D;
	class YumeVertexBuffer;

	class YumeAPIExport LightPropagationVolume : public YumeBase
	{
	public:
		LightPropagationVolume();
		virtual ~LightPropagationVolume();

		void Create(unsigned volumeSize);
		void Render();

		void Inject(const DirectX::XMFLOAT3& lpvMin,const DirectX::XMFLOAT3& lpvMax);
		void Normalize();

		void Propagate(unsigned num_iterations);
		void Propagate();

		void SetPropagateParameters();

		void SetModelMatrix(const DirectX::XMFLOAT4X4& model,const DirectX::XMFLOAT3& lpvMin,const DirectX::XMFLOAT3& lpvMax);

		void BindLPVTextures();

		SharedPtr<YumeTexture2D> GetLPVAccumR() const { return lpv_accum_r_; }
		SharedPtr<YumeTexture2D> GetLPVAccumG() const { return lpv_accum_g_; }
		SharedPtr<YumeTexture2D> GetLPVAccumB() const { return lpv_accum_b_; }

		YumeGeometry* GetLPVVolume() const { return lpv_volume_geo_; }
		
		Texture2DPtr lpvTextures_[10];
	private:
		unsigned volume_size_;
		unsigned curr_;
		unsigned next_;
		unsigned iterations_rendered_;

		DirectX::XMFLOAT4X4 world_to_lpv_;

		SharedPtr<YumeTexture2D> lpv_r_[2];
		SharedPtr<YumeTexture2D> lpv_g_[2];
		SharedPtr<YumeTexture2D> lpv_b_[2];

		SharedPtr<YumeTexture2D> lpv_accum_r_;
		SharedPtr<YumeTexture2D> lpv_accum_g_;
		SharedPtr<YumeTexture2D> lpv_accum_b_;

		SharedPtr<YumeTexture2D> lpv_inject_counter_;

		SharedPtr<YumeVertexBuffer> lpv_volume_;
		SharedPtr<YumeGeometry>		lpv_volume_geo_;

		
		YumeShaderVariation* injectVs_;
		YumeShaderVariation* injectGs_;
		YumeShaderVariation* injectPs_;

		YumeShaderVariation* propogateVs_;
		YumeShaderVariation* propogatePs_;
		YumeShaderVariation* propogateGs_;

		YumeShaderVariation* normalizePs_;
	};
}


//----------------------------------------------------------------------------
#endif
