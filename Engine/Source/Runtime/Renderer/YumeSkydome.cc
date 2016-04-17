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
#include "YumeSkydome.h"
#include "YumeRHI.h"


namespace YumeEngine
{
	YumeSkydome::YumeSkydome()
	{

	}

	YumeSkydome::~YumeSkydome()
	{

	}

	void YumeSkydome::Setup(const YumeString& mesh)
	{
		YumeMesh::Load(mesh);

		DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world,I);
		set_world(world);

		skyMap_ = gYume->pResourceManager->PrepareResource<YumeTexture2D>("Textures/sunny_day.jpg");


		for(auto i = meshes_.begin(); i != meshes_.end(); ++i)
			i->diffuse_tex = skyMap_;

		skyVs_ = gYume->pRHI->GetShader(VS,"LPV/MeshSkydome");
		skyPs_ = gYume->pRHI->GetShader(PS,"LPV/MeshSkydome");




	}

	void YumeSkydome::Render()
	{
		gYume->pRHI->BindDepthStateDisable();
		gYume->pRHI->SetShaders(skyVs_,skyPs_,0);
		YumeMesh::Render();
		gYume->pRHI->BindDepthStateEnable();
	}

}
