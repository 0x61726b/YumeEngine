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
#include "YumeHeaders.h"
#include "Light.h"
#include "YumeRHI.h"

#include "Scene.h"
#include "Math/YumeColor.h"

#include "RenderPass.h"

using namespace DirectX;

namespace YumeEngine
{
	Light::Light():
		SceneNode(GT_LIGHT),
		color_(YumeColor(1,1,1,1))
	{
	}

	Light::~Light()
	{
	}

	void Light::UpdateLightParameters()
	{
		XMMATRIX lightView = XMMatrixLookToLH(XMLoadFloat4(&GetPosition()),XMLoadFloat4(&GetDirection()),XMLoadFloat4(&GetRotation()));
		XMMATRIX lightProj = gYume->pRenderer->MakeProjection();

		XMMATRIX light_vp = lightView * lightProj;

		XMMATRIX lightViewProjInv = XMMatrixInverse(nullptr,light_vp);

		RenderPass* dp = gYume->pRenderer->GetDefaultPass();

		gYume->pRHI->SetShaderParameter("main_light_pos",XMFLOAT3(GetPosition().x,GetPosition().y,GetPosition().z));
		gYume->pRHI->SetShaderParameter("light_vp",light_vp);
		gYume->pRHI->SetShaderParameter("light_vp_inv",lightViewProjInv);
		gYume->pRHI->SetShaderParameter("light_mvp",light_vp);


		DirectX::XMMATRIX tex
			(
			0.5f,0.0f,0.0f,0.0f,
			0.0f,-0.5f,0.0f,0.0f,
			0.f,0.f,0.5f,0.0f,
			0.5f,0.5f,0.5f,1.0f
			);

		gYume->pRHI->SetShaderParameter("light_vp_tex",light_vp * tex);


	}
}
