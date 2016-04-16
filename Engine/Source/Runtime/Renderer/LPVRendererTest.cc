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
#include "YumeRenderPipeline.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"
#include "YumeRHI.h"
#include "YumeRenderer.h"
#include "YumeModel.h"
#include "YumeGeometry.h"
#include "Core/YumeFile.h"

#include "YumeStaticModel.h"

#include "AssimpMesh.h"



//#define RENDER_RSM_TO_BACKBUFFER
//#define SET_DEBUG_LIGHT_CAM
//#define INJECT_ONLY

using namespace DirectX;

namespace YumeEngine
{
	LPVRenderer::LPVRenderer()
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
		moveScale(2)
	{
		rhi_ = gYume->pRHI;
		renderer_ = gYume->pRenderer;

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

		mesh_ = new YumeMesh;
		mesh_->Load(gYume->pResourceManager->GetFullPath("Models/Cornell/cornellbox.obj"));


		XMMATRIX I = DirectX::XMMatrixIdentity();
		XMFLOAT4X4 sceneWorld;
		XMStoreFloat4x4(&sceneWorld,I);

		mesh_->set_world(sceneWorld);



		updateRsm_ = true;

		zNear = .2f;
		zFar = 100000.f;

		int sceneWidth = gYume->pRHI->GetWidth();
		int sceneHeight = gYume->pRHI->GetHeight();
		camera_.SetViewParams(XMVectorSet(0,5,-10,0),XMVectorSet(0,0,0,0));
		FLOAT aspect_ratio = static_cast<FLOAT>(gYume->pRHI->GetWidth()) / static_cast<FLOAT>(gYume->pRHI->GetHeight());
		camera_.SetProjParams(static_cast<FLOAT>(DirectX::XM_PI / 4),aspect_ratio,zNear,zFar);


		XMMATRIX view = camera_.GetViewMatrix();
		XMStoreFloat4x4(&view_,view);
		XMMATRIX proj = camera_.GetProjMatrix();
		XMStoreFloat4x4(&proj_,proj);

#ifdef RENDER_RSM_TO_BACKBUFFER
		meshVs_ = gYume->pRHI->GetShader(VS,"LPV/Test");
		meshPs_ = gYume->pRHI->GetShader(PS,"LPV/Test");
#else
		meshVs_ = gYume->pRHI->GetShader(VS,"LPV/Mesh");
		meshPs_ = gYume->pRHI->GetShader(PS,"LPV/Mesh");

		triangleVs_ = gYume->pRHI->GetShader(VS,"LPV/fs_triangle");
		deferredLpvPs_ = gYume->pRHI->GetShader(PS,"LPV/deferred_lpv");

		noiseTex_ = gYume->pResourceManager->PrepareResource<YumeTexture2D>("Textures/noise.dds");
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


		bbMin = mesh_->bb_min();
		bbMax = mesh_->bb_max();



		lpv_.Create(32);
		lpv_.SetLPVRenderer(this);
		dirLightUp = XMVectorSet(-1,0,0,0);

		dir_light_.position = XMFLOAT4(0,20,0,1);
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

		triangle_ = gYume->pRenderer->GetFsTriangle();

		UpdateMeshBb(*mesh_);
	}

	void LPVRenderer::SetCameraParameters(bool shadowPass)
	{
		XMMATRIX view = XMLoadFloat4x4(&view_);
		XMMATRIX proj = XMLoadFloat4x4(&proj_);
		XMMATRIX world = XMLoadFloat4x4(&world_);

		XMMATRIX vp = view * proj;
		XMMATRIX vpInv = XMMatrixInverse(nullptr,vp);

		XMFLOAT4 cameraPos;

		if(shadowPass)
			XMStoreFloat4(&cameraPos,XMLoadFloat4(&dir_light_.position));
		else
			XMStoreFloat4(&cameraPos,camera_.GetEyePt());

		gYume->pRHI->SetShaderParameter("vp",vp);
		gYume->pRHI->SetShaderParameter("vp_inv",vpInv);
		gYume->pRHI->SetShaderParameter("camera_pos",cameraPos);
		gYume->pRHI->SetShaderParameter("z_far",zFar);
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

		DirectX::XMVECTOR d = DirectX::XMVectorSubtract(
			XMLoadFloat3(&bbMax),XMLoadFloat3(&bbMin));

		FLOAT s;
		DirectX::XMStoreFloat(&s,DirectX::XMVector3Length(d));
		s /= 100.f;

		

		camera_.FrameMove(timeStep);

		float lightMove = 2 * timeStep;
		float gi = 1;

		camera_.SetScalers(0.0099f,moveScale);

		if(input->GetKeyDown(KEY_SHIFT))
		{
			lightMove *= -1;
			gi *= -1;
			moveScale = 5.5f;
			
		}

		if(input->GetKeyDown(KEY_I))
			dir_light_.position.x += lightMove;
		if(input->GetKeyDown(KEY_O))
			dir_light_.position.y += lightMove;
		if(input->GetKeyDown(KEY_P))
			dir_light_.position.z += lightMove;

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

			lpv_.Inject(bbMin,bbMax);

			lpv_.Normalize();

			lpv_.Propagate(number_it);

			updateRsm_ = false;
		}
#endif
#ifndef INJECT_ONLY
		//
		RenderSceneToGBuffer();

		lpv_.BindLPVTextures();


		{
			RHIEvent e("Mips");
			gYume->pRHI->GenerateMips(RsmLinearDepth_);
			gYume->pRHI->GenerateMips(SceneLinearDepth_);
		}

		rhi_->SetViewport(IntRect(0,0,gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight()));

		{
			RHIEvent e("DeferredRenderer");


			rhi_->BindSampler(PS,2,1,2); //Shadow
			rhi_->BindSampler(PS,1,1,3); //LPV


			rhi_->SetShaders(triangleVs_,deferredLpvPs_,0);

			rhi_->SetTexture(2,SceneColors_);
			rhi_->SetTexture(3,SceneSpecular_);
			rhi_->SetTexture(4,SceneNormals_);
			rhi_->SetTexture(5,SceneLinearDepth_);
			rhi_->SetTexture(6,RsmLinearDepth_);
			rhi_->SetTexture(14,noiseTex_);


			//lpv_parameters
			DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
			DirectX::XMFLOAT4X4 i;
			DirectX::XMStoreFloat4x4(&i,I);
			lpv_.SetModelMatrix(i,bbMin,bbMax);
			//Future me : this line crashes because constant buffer layout is being read wrong. FIX!1

			//light_ps
			SetDeferredLightParameters();
			SetGIParameters();

			rhi_->SetShaderParameter("scene_dim_max",XMFLOAT4(bbMax.x,bbMax.y,bbMax.z,1.0f));
			rhi_->SetShaderParameter("scene_dim_min",XMFLOAT4(bbMin.x,bbMin.y,bbMin.z,1.0f));

			UpdateCameraParameters();
			SetCameraParameters(false);

			rhi_->BindResetRenderTargets(6);
			rhi_->BindBackbuffer();

			rhi_->Clear(CLEAR_COLOR);

			triangle_->Draw(gYume->pRHI);

			rhi_->BindResetTextures(0,13);

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

		rhi_->SetShaders(meshVs_,meshPs_);

		rhi_->BindSampler(PS,0,1,0); //0 is standard filter

		UpdateCameraParameters();

		DrawScene(false);

		rhi_->BindResetRenderTargets(4);
	}

	void LPVRenderer::UpdateCameraParameters()
	{
		XMMATRIX view = camera_.GetViewMatrix();

		XMStoreFloat4x4(&view_,view);
	}

	void LPVRenderer::SetInjectStageTextures()
	{
		//Bind RSM to inject
		gYume->pRHI->SetTexture(6,RsmLinearDepth_);
		gYume->pRHI->SetTexture(7,RsmColors_);
		gYume->pRHI->SetTexture(8,RsmNormals_);
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
		XMMATRIX lightProj = MakeProjection(zNear,zFar);

		XMMATRIX light_vp = lightView * lightProj;

		XMMATRIX lightViewProjInv = XMMatrixInverse(nullptr,light_vp);


		rhi_->SetShaderParameter("main_light",XMFLOAT3(dir_light_.position.x,dir_light_.position.y,dir_light_.position.z));
		rhi_->SetShaderParameter("light_vp",light_vp);
		rhi_->SetShaderParameter("light_vp_inv",lightViewProjInv);
	}

	void LPVRenderer::SetDeferredLightParameters()
	{
		XMMATRIX lightView = XMMatrixLookToLH(XMLoadFloat4(&dir_light_.position),XMLoadFloat4(&dir_light_.normal),dirLightUp);
		XMMATRIX lightProj = MakeProjection(zNear,zFar);

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
		XMMATRIX lightView = XMMatrixLookToLH(XMLoadFloat4(&dir_light_.position),XMLoadFloat4(&dir_light_.normal),dirLightUp);

		XMStoreFloat4x4(&view_,lightView);
		XMStoreFloat4x4(&proj_,MakeProjection(zNear,zFar));

	}

	void LPVRenderer::DrawScene(bool shadowPass)
	{
		SetCameraParameters(shadowPass);
		mesh_->Render();
	}

	void LPVRenderer::DrawPlane(const XMVECTOR& Pos,const XMVECTOR& Scale,const XMVECTOR& color)
	{
		XMMATRIX world = DirectX::XMMatrixIdentity();
		XMMATRIX translate = XMMatrixTranslationFromVector(Pos);
		XMMATRIX scale = XMMatrixScalingFromVector(Scale);
		world = world * translate * scale;

		XMStoreFloat4x4(&world_,world);


		materialDiffuse_ = color;
		SetMaterialParameters();

		gYume->pRHI->SetShaderParameter("world",world);

		cornell_->Draw(gYume->pRHI);
	}

	void LPVRenderer::LoadMaterials(const YumeString& fileName)
	{
		String useFileName = fileName;
		if(useFileName.Trimmed().empty() && model_)
		{
			String path,file,extension;
			SplitPath(model_->GetModel()->GetName(),path,file,extension);
			useFileName = path + file + ".txt";
		}


		SharedPtr<YumeFile> file = gYume->pResourceManager->GetFile(useFileName);
		if(!file)
			return;

		unsigned index = 0;
		while(!file->Eof() && index < model_->GetNumGeometries())
		{
			YumeMaterial* material = gYume->pResourceManager->PrepareResource<YumeMaterial>(file->ReadLine());
			if(material)
				materials_.push_back(material);

			++index;
		}
	}

	void LPVRenderer::DrawBox(const XMVECTOR& Pos,const XMVECTOR& Scale)
	{
		XMMATRIX world = DirectX::XMMatrixIdentity();
		XMMATRIX translate = XMMatrixTranslationFromVector(Pos);
		XMMATRIX scale = XMMatrixScalingFromVector(Scale);
		world = world * translate * scale;

		XMStoreFloat4x4(&world_,world);


		materialDiffuse_ = XMVectorSet(1,0,0,1);
		SetMaterialParameters();

		gYume->pRHI->SetShaderParameter("world",world);
		plane_->Draw(gYume->pRHI);
	}

	void LPVRenderer::UpdateMeshBb(YumeMesh& mesh)
	{
		DirectX::XMVECTOR v_bb_min = XMLoadFloat3(&mesh.bb_min());
		DirectX::XMVECTOR v_bb_max = XMLoadFloat3(&mesh.bb_max());

		DirectX::XMVECTOR v_diag = DirectX::XMVectorSubtract(v_bb_max,v_bb_min);

		DirectX::XMVECTOR delta = DirectX::XMVectorScale(v_diag,0.05f);

		v_bb_min = DirectX::XMVectorSubtract(v_bb_min,delta);
		v_bb_max = DirectX::XMVectorAdd(v_bb_max,delta);

		v_bb_min = DirectX::XMVectorSetW(v_bb_min,1.f);
		v_bb_max = DirectX::XMVectorSetW(v_bb_max,1.f);

		auto world = DirectX::XMLoadFloat4x4(&mesh.world());
		v_bb_min = DirectX::XMVector4Transform(v_bb_min,world);
		v_bb_max = DirectX::XMVector4Transform(v_bb_max,world);

		DirectX::XMStoreFloat3(&bbMin,v_bb_min);
		DirectX::XMStoreFloat3(&bbMax,v_bb_max);
	}

	DirectX::XMMATRIX LPVRenderer::MakeProjection(float z_near,float z_far)
	{
		float n = z_near;
		float f = z_far;

		float q = f/(f-n);

		return DirectX::XMMATRIX
			(
			1.f,0.f,0.f,0.f,
			0.f,1.f,0.f,0.f,
			0.f,0.f,q,1.f,
			0.f,0.f,-q*n,0.f
			);
	}

}
