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
#include "SparseVoxelOctree.h"

#include "YumeTexture3D.h"
#include "YumeRHI.h"
#include "RenderPass.h"
#include "RenderCall.h"
#include "Light.h"
#include "Scene.h"
namespace YumeEngine
{
	SparseVoxelOctree::SparseVoxelOctree()
		:last_bound_(0)
	{
	}

	SparseVoxelOctree::~SparseVoxelOctree()
	{
	}

	void SparseVoxelOctree::Create(unsigned volumeSize)
	{
		volume_size_ = volumeSize;

		vNormal = gYume->pRenderer->GetDefaultPass()->GetTextureByName("UAV_NORMAL");

		vRho = gYume->pRenderer->GetDefaultPass()->GetTextureByName("UAV_RHO");


		DirectX::XMStoreFloat4x4(&worldToSvo_,DirectX::XMMatrixIdentity());

	}

	void SparseVoxelOctree::Filter()
	{
		gYume->pRHI->GenerateMips(vNormal);
		gYume->pRHI->GenerateMips(vRho);
	}

	void SparseVoxelOctree::Inject()
	{
		RHIEvent e("SvoInject");

		TexturePtr uavs[] ={vRho};

		YumeRHI* r = gYume->pRHI;

		r->SetRenderTargetsAndUAVs(0,1,1,uavs);

		float clear_color[] ={0.0f,0.0f,0.0f,0.0f};

		RenderPass* d = gYume->pRenderer->GetDefaultPass();

		TexturePtr rsmDepth = d->GetTextureByName("RSM_LINEARDEPTH");
		TexturePtr rsmColors = d->GetTextureByName("RSM_COLORS");
		TexturePtr rsmNormals = d->GetTextureByName("RSM_NORMALS");

		YumeVector<TexturePtr>::type allTextures = gYume->pRenderer->GetFreeTextures();
		allTextures[6] = rsmDepth;
		allTextures[7] = rsmColors;
		allTextures[8] = rsmNormals;

		UINT num_vpls = 1024 * 1024;


		r->PSBindSRV(6,3,allTextures);

		YumeShaderVariation* vsInject = r->GetShader(VS,"svo_inject","vs_svo_inject","vs_svo_inject");
		YumeShaderVariation* psInject = r->GetShader(PS,"svo_inject","ps_svo_inject","ps_svo_inject");

		r->SetShaders(vsInject,psInject,0);

		Light* light = static_cast<Light*>(gYume->pRenderer->GetScene()->GetDirectionalLight());

		const DirectX::XMFLOAT4& pos = light->GetPosition();
		const DirectX::XMFLOAT4& dir = light->GetDirection();
		const YumeColor& color = light->GetColor();

		const unsigned fSize = 4 * 3 * 4;
		float f[fSize] ={
			pos.x,pos.y,pos.z,pos.w,
			dir.x,dir.y,dir.z,dir.w,
			color.r_,color.g_,color.b_,color.a_
		};

		gYume->pRHI->SetShaderParameter("main_light",f,4*3);

		DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4X4 i;
		DirectX::XMStoreFloat4x4(&i,I);
		SetModelMatrix(i,gYume->pRenderer->GetMinBb(),gYume->pRenderer->GetMaxBb());

		light->UpdateLightParameters();

		r->BindPsuedoBuffer();
		r->Draw(POINT_LIST,0,num_vpls);

		gYume->pRHI->BindResetTextures(6,3); //Start at 6 count 3


		TexturePtr clearUavs[1] ={0};
		r->SetRenderTargetsAndUAVs(0,1,1,clearUavs);
		
	}

	void SparseVoxelOctree::Voxelize(RenderCall* call,YumeGeometry* geo,bool clear)
	{
		ClearPs();

		YumeRHI* r = gYume->pRHI;

		r->SetViewport(IntRect(0,0,volume_size_,volume_size_));

		r->SetCullMode(CULL_NONE);

		if(clear)
		{
			float clear[4] ={0.f,0.f,0.f,0.f};
			r->ClearUAV(vNormal,clear);
			r->ClearUAV(vRho,clear);
		}


		TexturePtr uavs[2] ={vNormal,vRho};
		r->SetRenderTargetsAndUAVs(0,1,2,uavs);

		r->SetBlendMode(BlendMode::BLEND_MAX);

		YumeShaderVariation* vsVoxelize = r->GetShader(VS,"svo_voxelize","vs_svo_voxelize","vs_svo_voxelize");
		YumeShaderVariation* psVoxelize = r->GetShader(PS,"svo_voxelize","ps_svo_voxelize","ps_svo_voxelize");
		YumeShaderVariation* gsVoxelize = r->GetShader(GS,"svo_voxelize","gs_svo_voxelize","gs_svo_voxelize");

		r->SetShaders(vsVoxelize,psVoxelize,gsVoxelize);

		gYume->pRenderer->ApplyShaderParameters(call);

		/*DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4X4 i;
		DirectX::XMStoreFloat4x4(&i,I);
		SetModelMatrix(i,gYume->pRenderer->GetMinBb(),gYume->pRenderer->GetMaxBb());*/


		gYume->pRenderer->RenderScene();

		TexturePtr clearUavs[2] ={0,0};
		r->SetRenderTargetsAndUAVs(0,1,2,clearUavs);

	}

	void SparseVoxelOctree::ClearPs()
	{
		/*TexturePtr srv_null[] ={nullptr,nullptr};
		gYume->pRHI->PSBindSRV(7,2,srv_null);*/
		gYume->pRHI->BindResetTextures(7,2,true);
	}

	void SparseVoxelOctree::SetModelMatrix(const DirectX::XMFLOAT4X4& model,const DirectX::XMFLOAT3& svo_min,const DirectX::XMFLOAT3& svo_max)
	{
		DirectX::XMMATRIX m_model = DirectX::XMLoadFloat4x4(&model);

		DirectX::XMMATRIX model_inv = DirectX::XMMatrixInverse(nullptr,m_model);

		DirectX::XMVECTOR diag = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&svo_max),DirectX::XMLoadFloat3(&svo_min));

		DirectX::XMFLOAT3 d;
		DirectX::XMStoreFloat3(&d,diag);

		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(1.f / d.x,1.f / d.y,1.f / d.z);

		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(-svo_min.x,-svo_min.y,-svo_min.z);

		DirectX::XMMATRIX world_to_svo = model_inv * trans * scale;
		DirectX::XMStoreFloat4x4(&worldToSvo_,world_to_svo);

		svo_min_ = svo_min;
		svo_max_ = svo_max;

		DirectX::XMVECTOR v_svo_min = DirectX::XMLoadFloat3(&svo_min);
		DirectX::XMVECTOR v_svo_max = DirectX::XMLoadFloat3(&svo_max);

		v_svo_min = DirectX::XMVector3Transform(v_svo_min,m_model);
		v_svo_max = DirectX::XMVector3Transform(v_svo_max,m_model);

		RenderPass* defPass = gYume->pRenderer->GetDefaultPass();

		defPass->SetShaderParameter("world_to_svo",world_to_svo);

		DirectX::XMFLOAT4 svoMin;
		DirectX::XMFLOAT4 svoMax;
		DirectX::XMStoreFloat4(&svoMin,v_svo_min);
		DirectX::XMStoreFloat4(&svoMax,v_svo_max);

		defPass->SetShaderParameter("bb_min",svoMin);
		defPass->SetShaderParameter("bb_max",svoMax);

	}
}
