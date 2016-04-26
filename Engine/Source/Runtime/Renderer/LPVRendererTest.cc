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
#include "LPVRendererTest.h"

#include "YumeGeometry.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"
#include "YumeRHI.h"
#include "YumeGeometry.h"
#include "Core/YumeFile.h"

#include "YumeMiscRenderer.h"

#include "AssimpMesh.h"



//#define RENDER_RSM_TO_BACKBUFFER
//#define SET_DEBUG_LIGHT_CAM
//#define INJECT_ONLY

using namespace DirectX;

namespace YumeEngine
{
	LPVRenderer::LPVRenderer(YumeMiscRenderer* mi)
		: cornell_(0),
		meshVs_(0),
		meshPs_(0),
		yaw_(0.0f),
		pitch_(0.0f),
		dirLightFlux(XMVectorSet(1,1,1,1)),
		gi_scale(1),
		lpv_flux_amplifier(4),
		debug_gi(false),
		number_it(64),
		moveScale(0.2f),
		misc_(mi)
	{
		rhi_ = gYume->pRHI;

		XMMATRIX I = DirectX::XMMatrixIdentity();
		XMStoreFloat4x4(&world_,I);
		XMStoreFloat4x4(&view_,I);
		XMStoreFloat4x4(&proj_,I);

	}

	LPVRenderer::~LPVRenderer()
	{
	}

	void LPVRenderer::Setup()
	{


		updateRsm_ = true;


		int sceneWidth = gYume->pRHI->GetWidth();
		int sceneHeight = gYume->pRHI->GetHeight();
		misc_->GetCamera()->SetViewParams(XMVectorSet(0,5,-10,0),XMVectorSet(0,0,0,0));
		FLOAT aspect_ratio = static_cast<FLOAT>(gYume->pRHI->GetWidth()) / static_cast<FLOAT>(gYume->pRHI->GetHeight());
		misc_->GetCamera()->SetProjParams(static_cast<FLOAT>(DirectX::XM_PI / 4),aspect_ratio,misc_->zNear,misc_->zFar);


		XMMATRIX view = misc_->GetCamera()->GetViewMatrix();
		XMStoreFloat4x4(&view_,view);
		XMMATRIX proj = misc_->GetCamera()->GetProjMatrix();
		XMStoreFloat4x4(&proj_,proj);

#ifdef RENDER_RSM_TO_BACKBUFFER
		meshVs_ = gYume->pRHI->GetShader(VS,"LPV/Test");
		meshPs_ = gYume->pRHI->GetShader(PS,"LPV/Test");
#else
		meshVs_ = gYume->pRHI->GetShader(VS,"LPV/Mesh");
		meshPs_ = gYume->pRHI->GetShader(PS,"LPV/Mesh");

		triangleVs_ = gYume->pRHI->GetShader(VS,"LPV/fs_triangle");
		deferredLpvPs_ = gYume->pRHI->GetShader(PS,"LPV/deferred_lpv");


#endif

		RsmColors_ = gYume->pRHI->CreateTexture2D();
		RsmColors_->SetName("RSM_COLORS");
		RsmColors_->SetSize(1024,1024,gYume->pRHI->GetRGBAFloat16FormatNs(),TextureUsage::TEXTURE_RENDERTARGET);

		RsmNormals_ = gYume->pRHI->CreateTexture2D();
		RsmNormals_->SetName("RSM_NORMALS");
		RsmNormals_->SetSize(1024,1024,24,TextureUsage::TEXTURE_RENDERTARGET); //DXGI_FORMAT_R10G10B10A2_UNORM

		RsmDummySpec_ = gYume->pRHI->CreateTexture2D();
		RsmDummySpec_->SetName("RSM_DSPEC");
		RsmDummySpec_->SetSize(1024,1024,gYume->pRHI->GetRGBAFloat16FormatNs(),TextureUsage::TEXTURE_RENDERTARGET); //DXGI_FORMAT_R10G10B10A2_UNORM


		RsmLinearDepth_ = gYume->pRHI->CreateTexture2D();
		RsmLinearDepth_->SetName("RSM_LINEARDEPTH");
		RsmLinearDepth_->SetSize(1024,1024,16,TextureUsage::TEXTURE_RENDERTARGET,1,10); //DXGI_FORMAT_R32G32_FLOAT


		RsmDepthStencil_ = gYume->pRHI->CreateTexture2D();
		RsmDepthStencil_->SetName("RSM_DEPTHSTENCIL");
		RsmDepthStencil_->SetSize(1024,1024,39,TextureUsage::TEXTURE_DEPTHSTENCIL); //DXGI_FORMAT_R32_TYPELESS


		gYume->pRHI->CreateStandardSampler();


		lpv_.Create(32);
		lpv_.SetLPVRenderer(this);
		dirLightUp = XMVectorSet(-1,0,0,0);


		initialLightPos = XMFLOAT3(0,0,0);

		dir_light_.normal = XMFLOAT4(0,-1,0,0);
		dir_light_.color = XMFLOAT4(1,1,1,1);

		{
			SceneColors_ = gYume->pRHI->CreateTexture2D();
			SceneColors_->SetName("SCENE_COLORS");
			SceneColors_->SetSize(sceneWidth,sceneHeight,gYume->pRHI->GetRGBAFloat16FormatNs(),TextureUsage::TEXTURE_RENDERTARGET);

			SceneNormals_ = gYume->pRHI->CreateTexture2D();
			SceneNormals_->SetName("SCENE_NORMALS");
			SceneNormals_->SetSize(sceneWidth,sceneHeight,24,TextureUsage::TEXTURE_RENDERTARGET); //DXGI_FORMAT_R10G10B10A2_UNORM

			SceneSpecular_ = gYume->pRHI->CreateTexture2D();
			SceneSpecular_->SetName("SCENE_SPECULAR");
			SceneSpecular_->SetSize(sceneWidth,sceneHeight,gYume->pRHI->GetRGBAFloat16FormatNs(),TextureUsage::TEXTURE_RENDERTARGET); //DXGI_FORMAT_R10G10B10A2_UNORM


			SceneLinearDepth_ = gYume->pRHI->CreateTexture2D();
			SceneLinearDepth_->SetName("SCENE_LINEARDEPTH");
			SceneLinearDepth_->SetSize(sceneWidth,sceneHeight,16,TextureUsage::TEXTURE_RENDERTARGET,1,10); //DXGI_FORMAT_R32G32_FLOAT
		}



		materialDiffuse_ = XMVectorSet(0.8353f,0.60f,0.89f,1.0f);

		triangle_ = misc_->GetFsTriangle();


	}

	void LPVRenderer::SetGIParameters()
	{
		gYume->pRHI->SetShaderParameter("gi_scale",gi_scale);
		gYume->pRHI->SetShaderParameter("vpl_scale",1.0f);
		gYume->pRHI->SetShaderParameter("lpv_flux_amplifier",lpv_flux_amplifier);
		gYume->pRHI->SetShaderParameter("num_vpls",0.0f);
		gYume->pRHI->SetShaderParameter("debug_gi",debug_gi);
	}

	void LPVRenderer::SetMaterialParameters()
	{
		XMFLOAT4 materialDiffuse;
		XMStoreFloat4(&materialDiffuse,materialDiffuse_);

		gYume->pRHI->SetShaderParameter("diffuse_color",materialDiffuse);
		gYume->pRHI->SetShaderParameter("specular_color",Vector4(0.35f,0.35f,0.35f,1));
		gYume->pRHI->SetShaderParameter("emissive_color",Vector4(0,0,0,1));
		gYume->pRHI->SetShaderParameter("has_diffuse_tex",false);
		gYume->pRHI->SetShaderParameter("has_normal_tex",false);
		gYume->pRHI->SetShaderParameter("has_specular_tex",false);
		gYume->pRHI->SetShaderParameter("has_alpha_tex",false);
		gYume->pRHI->SetShaderParameter("shading_mode",1.0f);
		gYume->pRHI->SetShaderParameter("roughness",0.35f);
		gYume->pRHI->SetShaderParameter("refractive_index",1.0f);
	}


	void LPVRenderer::Update(float timeStep)
	{
		YumeInput* input = gYume->pInput;



		float lightMove = 2 * timeStep;
		float gi = 1;

		if(input->GetKeyDown(KEY_SHIFT))
		{
			lightMove *= -1;
			gi *= -1;
			moveScale = 5.5f;

		}

		if(input->GetKeyDown(KEY_I))
		{
			dir_light_.position.x += lightMove;
			updateRsm_ = true;
		}
		if(input->GetKeyDown(KEY_O))
		{
			dir_light_.position.y += lightMove;
			updateRsm_ = true;
		}
		if(input->GetKeyDown(KEY_P))
		{
			dir_light_.position.z += lightMove;
			updateRsm_ = true;
		}



		if(input->GetKeyPress(KEY_J))
		{
			gi_scale += gi;
			gi_scale = Clamp(gi_scale,0.0f,1000.0f);
		}
		if(input->GetKeyPress(KEY_K))
		{
			lpv_flux_amplifier += gi;
			lpv_flux_amplifier = Clamp(lpv_flux_amplifier,0.0f,1000.0f);
			updateRsm_ = true;
		}
		if(input->GetKeyPress(KEY_L))
			debug_gi = !debug_gi;

		if(input->GetKeyDown(KEY_T))
		{
			number_it += 1;
			number_it = Clamp(number_it,0.0f,64.0f);
			updateRsm_ = true;
		}

	}

	void LPVRenderer::Render()
	{
		if(updateRsm_)
		{
			//Render GI
			SetRSMCamera();
			RenderRSM();

			rhi_->BindResetRenderTargets(4);

#ifndef RENDER_RSM_TO_BACKBUFFER

			lpv_.Inject(misc_->GetMinBb(),misc_->GetMaxBb());

			lpv_.Normalize();

			lpv_.Propagate(number_it);

			updateRsm_ = false;
		}
#endif
#ifndef INJECT_ONLY
		//
		RenderSceneToGBuffer();


		{
			RHIEvent e("Mips");
			gYume->pRHI->GenerateMips(RsmLinearDepth_);
			gYume->pRHI->GenerateMips(SceneLinearDepth_);
		}

		rhi_->SetViewport(IntRect(0,0,gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight()));

		{
			RHIEvent e("DeferredRenderer");


			//rhi_->BindSampler(PS,2,1,2); //Shadow
			//rhi_->BindSampler(PS,1,1,3); //LPV


			rhi_->SetShaders(triangleVs_,deferredLpvPs_,0);

			SharedPtr<YumeTexture2D> accumr = lpv_.GetLPVAccumR();
			SharedPtr<YumeTexture2D> accumg = lpv_.GetLPVAccumG();
			SharedPtr<YumeTexture2D> accumb = lpv_.GetLPVAccumB();
			YumeTexture2D* textures[8] ={SceneColors_,SceneSpecular_,SceneNormals_,SceneLinearDepth_,RsmLinearDepth_,accumr,accumg,accumb};

			rhi_->PSBindSRV(2,8,textures);



			//lpv_parameters
			DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
			DirectX::XMFLOAT4X4 i;
			DirectX::XMStoreFloat4x4(&i,I);
			lpv_.SetModelMatrix(i,misc_->GetMinBb(),misc_->GetMaxBb());
			//Future me : this line crashes because constant buffer layout is being read wrong. FIX!1

			//light_ps
			SetDeferredLightParameters();
			SetGIParameters();

			rhi_->SetShaderParameter("scene_dim_max",XMFLOAT4(misc_->GetMaxBb().x,misc_->GetMaxBb().y,misc_->GetMaxBb().z,1.0f));
			rhi_->SetShaderParameter("scene_dim_min",XMFLOAT4(misc_->GetMinBb().x,misc_->GetMinBb().y,misc_->GetMinBb().z,1.0f));

			misc_->SetCameraParameters(false);

			rhi_->BindResetRenderTargets(6);
			rhi_->SetDepthStencil((YumeTexture2D*)0);
			rhi_->SetRenderTarget(0,misc_->GetRenderTarget());
			rhi_->ClearRenderTarget(0,CLEAR_COLOR);
			/*rhi_->BindBackbuffer();
			rhi_->Clear(CLEAR_COLOR);*/


			triangle_->Draw(gYume->pRHI);

			rhi_->BindResetRenderTargets(1);
			/*rhi_->BindResetTextures(0,13);*/

		}

#endif
	}

	void LPVRenderer::RenderSceneToGBuffer()
	{
		RHIEvent e("GBuffer");

		rhi_->SetRenderTarget(0,SceneColors_);
		rhi_->SetRenderTarget(1,SceneNormals_);
		rhi_->SetRenderTarget(2,SceneLinearDepth_);
		rhi_->SetRenderTarget(3,SceneSpecular_);

		rhi_->BindDefaultDepthStencil();

		rhi_->SetViewport(IntRect(0,0,gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight()));

		rhi_->ClearDepthStencil(CLEAR_DEPTH,1.0f,0.0f);
		rhi_->ClearRenderTarget(0,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,1));
		rhi_->ClearRenderTarget(1,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,1));
		rhi_->ClearRenderTarget(2,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,1));
		rhi_->ClearRenderTarget(3,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,1));

		rhi_->SetBlendMode(BLEND_REPLACE);

		//Render sky first xD
		misc_->RenderSky();

		rhi_->SetShaders(meshVs_,meshPs_);

		rhi_->BindSampler(PS,0,1,0); //0 is standard filter

		DrawScene(false);

		rhi_->BindResetRenderTargets(4);
	}



	void LPVRenderer::SetInjectStageTextures()
	{
		YumeTexture2D* textures[3] ={RsmLinearDepth_,RsmColors_,RsmNormals_};

		gYume->pRHI->VSBindSRV(6,3,textures);
	}

	void LPVRenderer::RenderLPV()
	{

	}

	void LPVRenderer::RenderRSM()
	{
		RHIEvent e("RenderRSM");

		rhi_->SetCullMode(CULL_CCW);
#ifndef RENDER_RSM_TO_BACKBUFFER
		rhi_->SetRenderTarget(0,RsmColors_);
		rhi_->SetRenderTarget(1,RsmNormals_);
		rhi_->SetRenderTarget(2,RsmLinearDepth_);
		rhi_->SetRenderTarget(3,RsmDummySpec_);
		rhi_->SetDepthStencil(RsmDepthStencil_);
#endif
		rhi_->SetDepthTest(CMP_ALWAYS);
		rhi_->SetColorWrite(true);
		rhi_->SetFillMode(FILL_SOLID);
		rhi_->SetClipPlane(false);
		rhi_->SetScissorTest(false);

#ifndef RENDER_RSM_TO_BACKBUFFER
		rhi_->ClearRenderTarget(0,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,1));
		rhi_->ClearRenderTarget(1,CLEAR_COLOR | CLEAR_DEPTH,YumeColor(0,0,0,1));
		rhi_->ClearRenderTarget(2,CLEAR_DEPTH,YumeColor(10000,10000,10000,10000));
		rhi_->ClearDepthStencil(CLEAR_DEPTH,1.0f,0.0f);
		rhi_->SetViewport(IntRect(0,0,1024,1024));
#else
		rhi_->Clear(CLEAR_COLOR | CLEAR_DEPTH |CLEAR_STENCIL);
#endif

		rhi_->BindSampler(PS,0,1,0);
		rhi_->BindNullBlendState();

		rhi_->SetDepthTest(CMP_LESS);
		rhi_->SetStencilTest(true,CMP_ALWAYS,OP_KEEP,OP_KEEP,OP_INCR,OP_DECR);

		rhi_->SetShaders(meshVs_,meshPs_);

		DrawScene(true);
	}

	void LPVRenderer::SetLightParameters()
	{
		XMMATRIX lightView = XMMatrixLookToLH(XMLoadFloat4(&dir_light_.position),XMLoadFloat4(&dir_light_.normal),dirLightUp);
		XMMATRIX lightProj = misc_->MakeProjection();

		XMMATRIX light_vp = lightView * lightProj;

		XMMATRIX lightViewProjInv = XMMatrixInverse(nullptr,light_vp);


		rhi_->SetShaderParameter("main_light",XMFLOAT3(dir_light_.position.x,dir_light_.position.y,dir_light_.position.z));
		rhi_->SetShaderParameter("light_vp",light_vp);
		rhi_->SetShaderParameter("light_vp_inv",lightViewProjInv);
	}

	void LPVRenderer::SetDeferredLightParameters()
	{
		XMMATRIX lightView = XMMatrixLookToLH(XMLoadFloat4(&dir_light_.position),XMLoadFloat4(&dir_light_.normal),dirLightUp);
		XMMATRIX lightProj = misc_->MakeProjection();

		XMMATRIX light_vp = lightView * lightProj;

		XMMATRIX lightViewProjInv = XMMatrixInverse(nullptr,light_vp);

		rhi_->SetShaderParameter("light_vp_inv",lightViewProjInv);
		rhi_->SetShaderParameter("light_mvp",light_vp);


		DirectX::XMMATRIX tex
			(
			0.5f,0.0f,0.0f,0.0f,
			0.0f,-0.5f,0.0f,0.0f,
			0.f,0.f,0.5f,0.0f,
			0.5f,0.5f,0.5f,1.0f
			);

		rhi_->SetShaderParameter("light_vp_tex",light_vp * tex);

		const unsigned fSize = 4 * 3 * 4;
		float f[fSize] ={
			dir_light_.position.x,dir_light_.position.y,dir_light_.position.z,dir_light_.position.w,
			dir_light_.normal.x,dir_light_.normal.y,dir_light_.normal.z,dir_light_.normal.w,
			dir_light_.color.x,dir_light_.color.y,dir_light_.color.z,dir_light_.color.w
		};

		rhi_->SetShaderParameter("main_light",f,4*3);


	}

	void LPVRenderer::SetRSMCamera()
	{


	}

	void LPVRenderer::DrawScene(bool shadowPass)
	{
		misc_->SetCameraParameters(shadowPass);
		misc_->RenderScene();
	}

	void LPVRenderer::SetInitialLPVPos(float x,float y,float z)
	{
		initialLightPos = XMFLOAT3(x,y,z);
	}

	void LPVRenderer::SetLPVPos(float x,float y,float z)
	{
		if(x != -1)
			dir_light_.position.x = initialLightPos.x + x;

		if(y != -1)
			dir_light_.position.y =  initialLightPos.y+ y;

		if(z != -1)
			dir_light_.position.z = initialLightPos.z + z;

		updateRsm_ = true;
	}

	void LPVRenderer::SetLightFlux(float f)
	{
		/*if(f < 1)
			return;

			dir_light_.color = XMFLOAT4(dir_light_.color.x * f,dir_light_.color.y * f,dir_light_.color.z * f,1.0f);
			updateRsm_ = true;*/
	}

}
