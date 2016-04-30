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
#include "LightPropagationVolume.h"
#include "YumeGeometry.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"
#include "YumeRHI.h"

#include "LPVRendererTest.h"

#include "RenderPass.h"
#include "YumeMiscRenderer.h"




#define NUM_VPLS 1024

#define Kappa

namespace YumeEngine
{

	struct lpv_vertex
	{
		Vector3 position;
		Vector3 texcoord;

		void init(Vector3& p,Vector3& t)
		{
			position = p;
			texcoord = t;
		}
	};


	LightPropagationVolume::LightPropagationVolume()
		: volume_size_(0)
	{
	}

	LightPropagationVolume::~LightPropagationVolume()
	{
	}

	void LightPropagationVolume::Create(unsigned volumeSize)
	{
		volume_size_ = volumeSize;
		UINT num_vertices = 6 * volume_size_;
		std::vector<lpv_vertex> data(num_vertices);

		for(size_t d = 0; d < volume_size_; ++d)
		{
			data[d*6+0].init(Vector3(-1.0f,-1.0f,0.0f),Vector3(0.0f,1.0f,static_cast<float>(d)));
			data[d*6+1].init(Vector3(-1.0f,1.0f,0.0f),Vector3(0.0f,0.0f,static_cast<float>(d)));
			data[d*6+2].init(Vector3(1.0f,-1.0f,0.0f),Vector3(1.0f,1.0f,static_cast<float>(d)));

			data[d*6+3].init(Vector3(1.0f,-1.0f,0.0f),Vector3(1.0f,1.0f,static_cast<float>(d)));
			data[d*6+4].init(Vector3(-1.0f,1.0f,0.0f),Vector3(0.0f,0.0f,static_cast<float>(d)));
			data[d*6+5].init(Vector3(1.0f,1.0f,0.0f),Vector3(1.0f,0.0f,static_cast<float>(d)));
		}

		lpv_volume_ = gYume->pRHI->CreateVertexBuffer();
		lpv_volume_->SetShadowed(true);
		lpv_volume_->SetSize(num_vertices,MASK_POSITION | MASK_TEXCOORD3);

		lpv_volume_->SetData(&data[0]);


		lpv_volume_geo_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		lpv_volume_geo_->SetVertexBuffer(0,lpv_volume_);
		lpv_volume_geo_->SetDrawRange(TRIANGLE_LIST,0,0,0,num_vertices);


		curr_ = 0;
		next_ = 1;

		iterations_rendered_ = 0;

#ifndef Kappa
		RenderTargetDesc lpvrgb;
		lpvrgb.Width = volumeSize;
		lpvrgb.Height = volumeSize;
		lpvrgb.Format = gYume->pRHI->GetRGBAFloat16FormatNs();
		lpvrgb.Usage = TEXTURE_RENDERTARGET;
		lpvrgb.Type = RT_OUTPUT | RT_INPUT ;
		lpvrgb.ArraySize = volumeSize;
		lpvrgb.ClearColor = YumeColor(0,0,0,0);
		lpvrgb.Mips = 1;

		//LPV_R_0 ID 0
		//LPV_G_0 ID 1
		//LPV_B_0 ID 2
		//LPV_R_1 ID 3
		//LPV_G_1 ID 4
		//LPV_B_1 ID 5
		//LPV_ACCUMR_0 ID 6
		//LPV_ACCUMG_0 ID 7
		//LPV_ACCUMB_0 ID 8
		//LPV_INJECT_COUNTER ID 9

		RenderCallPtr lpvInject = YumeAPINew RenderCall(CallType::LPV_INJECT,
			"LPV/LPVInject",
			"LPV/LPVInject",
			"LPV/LPVInject",
			"LPVInjectVs","LPVInjectPs","LPVInjectGs");

		RenderCallPtr lpvNormalize =  YumeAPINew RenderCall(CallType::LPV_NORMALIZE,
			"LPV/LPVPropagate",
			"LPV/LPVNormalize",
			"LPV/LPVPropagate",
			"LPVPropagateVs","LPVNormalizePs","LPVPropagateGs");

		RenderCallPtr lpvPropagate =  YumeAPINew RenderCall(CallType::LPV_PROPAGATE,
			"LPV/LPVPropagate",
			"LPV/LPVPropagate",
			"LPV/LPVPropagate",
			"LPVPropagateVs","LPVPropagatePs","LPVPropagateGs");

		lpvInject->SetMiscRenderingFlags(RF_NODEPTHSTENCIL);
		lpvNormalize->SetMiscRenderingFlags(RF_NODEPTHSTENCIL);
		lpvPropagate->SetMiscRenderingFlags(RF_NODEPTHSTENCIL);

		lpvNormalize->SetShaderParameter("gi_scale",1.0f);
		lpvNormalize->SetShaderParameter("lpv_flux_amplifier",1.0f);
		lpvNormalize->SetShaderParameter("debug_gi",false);

		lpvPropagate->SetShaderParameter("gi_scale",1.0f);
		lpvPropagate->SetShaderParameter("lpv_flux_amplifier",1.0f);
		lpvPropagate->SetShaderParameter("debug_gi",false);

		lpvInject->SetShaderParameter("light_vp",DirectX::XMMatrixIdentity());
		lpvInject->SetShaderParameter("light_vp_inv",DirectX::XMMatrixIdentity());
		lpvInject->SetShaderParameter("main_light_pos",DirectX::XMFLOAT3(0,0,0));

		lpvInject->SetPassName("LPVInject");
		lpvNormalize->SetPassName("LPVNormalize");
		lpvPropagate->SetPassName("LPVPropagate");

		lpvInject->SetIdentifier("LPVDeferred");
		lpvNormalize->SetIdentifier("LPVDeferred");
		lpvPropagate->SetIdentifier("LPVDeferred");

		int identifier = 0;
		for(size_t i=0; i < 2; ++i)
		{
			YumeString name = "LPV_R_";
			name.AppendWithFormat("%i",i);
			lpvrgb.Name = name;
			lpvrgb.Index = identifier;
			Texture2DPtr lpvr = lpvInject->AddTexture(lpvrgb);
			lpvTextures_[identifier++] = lpvr;

			name = "LPV_G_";
			name.AppendWithFormat("%i",i);
			lpvrgb.Name = name;
			lpvrgb.Index = identifier;
			Texture2DPtr lpvg = lpvInject->AddTexture(lpvrgb);
			lpvTextures_[identifier++] = lpvg;

			name = "LPV_B_";
			name.AppendWithFormat("%i",i);
			lpvrgb.Name = name;
			lpvrgb.Index = identifier;
			Texture2DPtr lpvb = lpvInject->AddTexture(lpvrgb);
			lpvTextures_[identifier++] = lpvb;
		}

		lpvrgb.Name = "LPV_ACCUM_R";
		lpvrgb.Index = identifier;
		Texture2DPtr lpvaccumr = lpvInject->AddTexture(lpvrgb);
		lpvTextures_[identifier++] = lpvaccumr;

		lpvrgb.Name = "LPV_ACCUM_G";
		lpvrgb.Index = identifier;
		Texture2DPtr lpvaccumg = lpvInject->AddTexture(lpvrgb);
		lpvTextures_[identifier++] = lpvaccumg;

		lpvrgb.Name = "LPV_ACCUM_B";
		lpvrgb.Index = identifier;
		Texture2DPtr lpvaccumb = lpvInject->AddTexture(lpvrgb);
		lpvTextures_[identifier++] = lpvaccumb;

		lpvrgb.Name = "LPV_INJECT_COUNTER";
		lpvrgb.Index = identifier;
		Texture2DPtr lpvinjectcounter = lpvInject->AddTexture(lpvrgb);
		lpvTextures_[identifier] = lpvinjectcounter;

		gYume->pRenderer->GetDefaultPass()->AddRenderCall(lpvInject);

		lpvNormalize->AddTextures(10,lpvTextures_);
		lpvPropagate->AddTextures(10,lpvTextures_);

		gYume->pRenderer->GetDefaultPass()->AddRenderCall(lpvNormalize);
		gYume->pRenderer->GetDefaultPass()->AddRenderCall(lpvPropagate);


#endif

	}

	void LightPropagationVolume::Inject(const DirectX::XMFLOAT3& lpvMin,const DirectX::XMFLOAT3& lpvMax)
	{
		RHIEvent e("LPV_Inject");

		unsigned int num_vpls = static_cast<unsigned int>(NUM_VPLS*NUM_VPLS);

		/*gYume->pRHI->BindResetTextures(7,3,true);*/

		gYume->pRHI->SetViewport(IntRect(0,0,volume_size_,volume_size_));

		gYume->pRHI->SetRenderTarget(0,lpv_r_[next_]);
		gYume->pRHI->SetRenderTarget(1,lpv_g_[next_]);
		gYume->pRHI->SetRenderTarget(2,lpv_b_[next_]);
		gYume->pRHI->SetRenderTarget(3,lpv_inject_counter_);

		gYume->pRHI->ClearRenderTarget(0,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(1,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(2,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(3,CLEAR_COLOR);

		//gYume->pRHI->BindSampler(VS,0,1,1); //1 is VPLFilter

		gYume->pRHI->SetDepthStencil((YumeRenderable*)0);

		gYume->pRHI->BindInjectBlendState();

		gYume->pRHI->SetShaders(injectVs_,injectPs_,injectGs_);

		DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4X4 i;
		DirectX::XMStoreFloat4x4(&i,I);
		SetModelMatrix(i,lpvMin,lpvMax);

		lpvRenderer_->SetLightParameters();
		lpvRenderer_->SetInjectStageTextures();

		gYume->pRHI->BindPsuedoBuffer();

		gYume->pRHI->Draw(POINT_LIST,0,num_vpls);

		gYume->pRHI->BindResetRenderTargets(4);
		gYume->pRHI->BindResetTextures(6,3); //Start at 6 count 3


	}

	void LightPropagationVolume::Render()
	{
		Normalize();
	}

	void LightPropagationVolume::Normalize()
	{
		RHIEvent e("LPV_Normalize");

		//std::swap(curr_,next_);

		//gYume->pRHI->SetViewport(IntRect(0,0,volume_size_,volume_size_));

		//gYume->pRHI->SetRenderTarget(0,lpv_r_[next_]);
		//gYume->pRHI->SetRenderTarget(1,lpv_g_[next_]);
		//gYume->pRHI->SetRenderTarget(2,lpv_b_[next_]);

		//gYume->pRHI->ClearRenderTarget(0,CLEAR_COLOR);
		//gYume->pRHI->ClearRenderTarget(1,CLEAR_COLOR);
		//gYume->pRHI->ClearRenderTarget(2,CLEAR_COLOR);

		//gYume->pRHI->BindInjectBlendState();

		//gYume->pRHI->SetShaders(propogateVs_,normalizePs_,propogateGs_);

		//lpvRenderer_->SetGIParameters();

		//YumeTexture2D* textures[4] ={lpv_r_[curr_],lpv_g_[curr_],lpv_b_[curr_],lpv_inject_counter_};
		//gYume->pRHI->PSBindSRV(7,4,textures);
		///*gYume->pRHI->SetTexture(7,lpv_r_[curr_]);
		//gYume->pRHI->SetTexture(8,lpv_g_[curr_]);
		//gYume->pRHI->SetTexture(9,lpv_b_[curr_]);
		//gYume->pRHI->SetTexture(10,lpv_inject_counter_);*/


		//static YumeVector<YumeVertexBuffer*>::type vertexBuffers(1);
		//static YumeVector<unsigned>::type elementMasks(1);
		//vertexBuffers[0] = lpv_volume_;
		//elementMasks[0] = lpv_volume_->GetElementMask();
		//gYume->pRHI->SetVertexBuffers(vertexBuffers,elementMasks);

		//gYume->pRHI->BindNullIndexBuffer();

		//gYume->pRHI->Draw(TRIANGLE_LIST,0,6 * volume_size_);

		//gYume->pRHI->BindResetRenderTargets(3);
		//gYume->pRHI->BindResetTextures(7,4,true);

	}

	void LightPropagationVolume::Propagate()
	{
		//std::swap(curr_,next_);

		//RHIEvent e("LPV_Propagate");

		//gYume->pRHI->SetRenderTarget(0,lpv_r_[next_]);
		//gYume->pRHI->SetRenderTarget(1,lpv_g_[next_]);
		//gYume->pRHI->SetRenderTarget(2,lpv_b_[next_]);
		//gYume->pRHI->SetRenderTarget(3,lpv_accum_r_);
		//gYume->pRHI->SetRenderTarget(4,lpv_accum_g_);
		//gYume->pRHI->SetRenderTarget(5,lpv_accum_b_);

		//gYume->pRHI->ClearRenderTarget(0,CLEAR_COLOR);
		//gYume->pRHI->ClearRenderTarget(1,CLEAR_COLOR);
		//gYume->pRHI->ClearRenderTarget(2,CLEAR_COLOR);


		//YumeTexture2D* textures[4] ={lpv_r_[curr_],lpv_g_[curr_],lpv_b_[curr_]};
		//gYume->pRHI->PSBindSRV(7,3,textures);


		///*gYume->pRHI->SetTexture(7,lpv_r_[curr_]);
		//gYume->pRHI->SetTexture(8,lpv_g_[curr_]);
		//gYume->pRHI->SetTexture(9,lpv_b_[curr_]);*/

		//lpvRenderer_->SetGIParameters();

		//gYume->pRHI->BindNullIndexBuffer();

		//lpv_volume_geo_->Draw(gYume->pRHI);

		//gYume->pRHI->BindResetRenderTargets(6);
		//gYume->pRHI->BindResetTextures(7,3,true);

	}

	void LightPropagationVolume::Propagate(unsigned num_iterations)
	{
		if(num_iterations == 0)
			return;

		gYume->pRHI->SetRenderTarget(0,lpv_accum_r_);
		gYume->pRHI->SetRenderTarget(1,lpv_accum_g_);
		gYume->pRHI->SetRenderTarget(2,lpv_accum_b_);

		gYume->pRHI->ClearRenderTarget(0,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(1,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(2,CLEAR_COLOR);

		gYume->pRHI->BindPropogateBlendState();

		gYume->pRHI->SetShaders(propogateVs_,propogatePs_,propogateGs_);


		for(int i=0; i < num_iterations; ++i)
		{
			gYume->pRHI->SetShaderParameter("iteration",(float)i);
			Propagate();
		}
	}

	void LightPropagationVolume::BindLPVTextures()
	{
		gYume->pRHI->SetTexture(7,lpv_accum_r_);
		gYume->pRHI->SetTexture(8,lpv_accum_g_);
		gYume->pRHI->SetTexture(9,lpv_accum_b_);
	}

	void LightPropagationVolume::SetPropagateParameters()
	{

	}

	void LightPropagationVolume::SetModelMatrix(const DirectX::XMFLOAT4X4& model,const DirectX::XMFLOAT3& lpvMin,const DirectX::XMFLOAT3& lpvMax)
	{
		DirectX::XMMATRIX model_inv = DirectX::XMMatrixInverse(nullptr,DirectX::XMLoadFloat4x4(&model));

		DirectX::XMVECTOR diag = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&lpvMax),DirectX::XMLoadFloat3(&lpvMin));

		DirectX::XMFLOAT3 d;
		DirectX::XMStoreFloat3(&d,diag);

		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(1.f/d.x,1.f/d.y,1.f/d.z);

		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(-lpvMin.x,-lpvMin.y,-lpvMin.z);

		DirectX::XMMATRIX world_to_lpv = model_inv * trans * scale;
		DirectX::XMStoreFloat4x4(&world_to_lpv_,world_to_lpv);

		RenderPass* rp = gYume->pRenderer->GetDefaultPass();

		rp->SetShaderParameter("world_to_lpv",world_to_lpv);
		rp->SetShaderParameter("lpv_size",(float)volume_size_);

	}

	void LightPropagationVolume::SetLPVRenderer(LPVRenderer* r)
	{
		lpvRenderer_ = r;
	}
}
