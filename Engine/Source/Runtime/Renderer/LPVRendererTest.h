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
#ifndef __LPVRendererTest_h__
#define __LPVRendererTest_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport LPVRenderer : public YumeBase
	{
	public:
		LPVRenderer();

		virtual ~LPVRenderer();

		void Setup();
		void Update();
		void Render();

		void SetCameraParameters();

	private:
		Matrix4 GetView(const Vector3& vEye, const Vector3& vAt, const Vector3& vUp, bool bRightHanded);
		Matrix4 GetProj(float nearr, float farr, float fieldOfView, float aspectRatio);
		Vector3 cameraPos_;

		YumeGeometry* cornell_;
		Matrix4 world_;
		Matrix4 view_;
		Matrix4 proj_;

		YumeShaderVariation* meshVs_;
		YumeShaderVariation* meshPs_;

		//RSM Render Targets
		SharedPtr<YumeTexture2D> RsmColors_;
		SharedPtr<YumeTexture2D> RsmNormals_;
		SharedPtr<YumeTexture2D> RsmLinearDepth_;
		SharedPtr<YumeTexture2D> RsmDummySpec_;

		SharedPtr<YumeTexture2D> RsmDepthStencil_;

		YumeRHI* rhi_;
		YumeRenderer* renderer_;
	};
}


//----------------------------------------------------------------------------
#endif
