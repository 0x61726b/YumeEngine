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
#include "YumeRenderPipeline.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"
#include "YumeRHI.h"
#include "YumeRenderer.h"
#include "YumeModel.h"

#include "LPVRendererTest.h"




#define NUM_VPLS 1024

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

		injectPs_ = gYume->pRHI->GetShader(PS,"LPV/LPVInject");
		injectVs_ = gYume->pRHI->GetShader(VS,"LPV/LPVInject");
		injectGs_ = gYume->pRHI->GetShader(GS,"LPV/LPVInject","");

		propogateVs_ = gYume->pRHI->GetShader(VS,"LPV/LPVPropogate");
		propogatePs_ = gYume->pRHI->GetShader(PS,"LPV/LPVPropogate");
		propogateGs_ = gYume->pRHI->GetShader(GS,"LPV/LPVPropogate","");

		normalizePs_ = gYume->pRHI->GetShader(PS,"LPV/LPVNormalize","");

		for(size_t i=0; i < 2; ++i)
		{
			lpv_r_[i] = gYume->pRHI->CreateTexture2D();
			YumeString debugName = "LPV_R_";
			debugName.AppendWithFormat("%i",i);
			lpv_r_[i]->SetName(debugName);
			lpv_r_[i]->SetSize(volumeSize,volumeSize,gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,volumeSize);

			lpv_g_[i] = gYume->pRHI->CreateTexture2D();
			debugName = "LPV_G_";
			debugName.AppendWithFormat("%i",i);
			lpv_g_[i]->SetName(debugName);
			lpv_g_[i]->SetSize(volumeSize,volumeSize,gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,volumeSize);

			lpv_b_[i] = gYume->pRHI->CreateTexture2D();
			debugName = "LPV_B_";
			debugName.AppendWithFormat("%i",i);
			lpv_b_[i]->SetName(debugName);
			lpv_b_[i]->SetSize(volumeSize,volumeSize,gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,volumeSize);
		}

		lpv_accum_r_ = gYume->pRHI->CreateTexture2D();
		lpv_accum_r_->SetName("LPV_ACCUM_R_");
		lpv_accum_r_->SetSize(volumeSize,volumeSize,gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,volumeSize);

		lpv_accum_g_ = gYume->pRHI->CreateTexture2D();
		lpv_accum_g_->SetName("LPV_ACCUM_G_");
		lpv_accum_g_->SetSize(volumeSize,volumeSize,gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,volumeSize);

		lpv_accum_b_ = gYume->pRHI->CreateTexture2D();
		lpv_accum_b_->SetName("LPV_ACCUM_B_");
		lpv_accum_b_->SetSize(volumeSize,volumeSize,gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,volumeSize);


		lpv_inject_counter_ = gYume->pRHI->CreateTexture2D();
		lpv_inject_counter_->SetName("LPV_INJECT_COUNTER");
		lpv_inject_counter_->SetSize(volumeSize,volumeSize,gYume->pRHI->GetFloat16FormatNs(),TEXTURE_RENDERTARGET,volumeSize);


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

		gYume->pRHI->BindSampler(VS,0,1,1); //1 is LPVFilter

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

		std::swap(curr_,next_);

		gYume->pRHI->SetViewport(IntRect(0,0,volume_size_,volume_size_));

		gYume->pRHI->SetRenderTarget(0,lpv_r_[next_]);
		gYume->pRHI->SetRenderTarget(1,lpv_g_[next_]);
		gYume->pRHI->SetRenderTarget(2,lpv_b_[next_]);

		gYume->pRHI->ClearRenderTarget(0,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(1,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(2,CLEAR_COLOR);

		gYume->pRHI->BindInjectBlendState();

		gYume->pRHI->SetShaders(propogateVs_,normalizePs_,propogateGs_);

		lpvRenderer_->SetGIParameters();

		YumeTexture2D* textures[4] = { lpv_r_[curr_],lpv_g_[curr_],lpv_b_[curr_],lpv_inject_counter_ };
		gYume->pRHI->PSBindSRV(7,4,textures);
		/*gYume->pRHI->SetTexture(7,lpv_r_[curr_]);
		gYume->pRHI->SetTexture(8,lpv_g_[curr_]);
		gYume->pRHI->SetTexture(9,lpv_b_[curr_]);
		gYume->pRHI->SetTexture(10,lpv_inject_counter_);*/


		static YumeVector<YumeVertexBuffer*>::type vertexBuffers(1);
		static YumeVector<unsigned>::type elementMasks(1);
		vertexBuffers[0] = lpv_volume_;
		elementMasks[0] = lpv_volume_->GetElementMask();
		gYume->pRHI->SetVertexBuffers(vertexBuffers,elementMasks);

		gYume->pRHI->BindNullIndexBuffer();

		gYume->pRHI->Draw(TRIANGLE_LIST,0,6 * volume_size_);

		gYume->pRHI->BindResetRenderTargets(3);
		gYume->pRHI->BindResetTextures(7,4,true);

	}

	void LightPropagationVolume::Propagate()
	{
		std::swap(curr_,next_);

		RHIEvent e("LPV_Propagate");

		gYume->pRHI->SetRenderTarget(0,lpv_r_[next_]);
		gYume->pRHI->SetRenderTarget(1,lpv_g_[next_]);
		gYume->pRHI->SetRenderTarget(2,lpv_b_[next_]);
		gYume->pRHI->SetRenderTarget(3,lpv_accum_r_);
		gYume->pRHI->SetRenderTarget(4,lpv_accum_g_);
		gYume->pRHI->SetRenderTarget(5,lpv_accum_b_);

		gYume->pRHI->ClearRenderTarget(0,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(1,CLEAR_COLOR);
		gYume->pRHI->ClearRenderTarget(2,CLEAR_COLOR);


		YumeTexture2D* textures[4] = { lpv_r_[curr_],lpv_g_[curr_],lpv_b_[curr_] };
		gYume->pRHI->PSBindSRV(7,3,textures);


		/*gYume->pRHI->SetTexture(7,lpv_r_[curr_]);
		gYume->pRHI->SetTexture(8,lpv_g_[curr_]);
		gYume->pRHI->SetTexture(9,lpv_b_[curr_]);*/

		lpvRenderer_->SetGIParameters();

		gYume->pRHI->BindNullIndexBuffer();
		
		lpv_volume_geo_->Draw(gYume->pRHI);

		gYume->pRHI->BindResetRenderTargets(6);
		gYume->pRHI->BindResetTextures(7,3,true);

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

		gYume->pRHI->SetShaderParameter("world_to_lpv",world_to_lpv);
		gYume->pRHI->SetShaderParameter("lpv_size",(float)volume_size_);
	}

	void LightPropagationVolume::SetLPVRenderer(LPVRenderer* r)
	{
		lpvRenderer_ = r;
	}
}
