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
#include "YumeMiscRenderer.h"

#include "LPVRendererTest.h"

#include "YumeRHI.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"

#include "YumePostProcess.h"
#include "YumeTimer.h"

using namespace DirectX;

namespace YumeEngine
{
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 V;
	};

	YumeMiscRenderer::YumeMiscRenderer()
		:
		zNear(.2f),
		zFar(100000.f)
	{
		rhi_ = gYume->pRHI ;
	}

	YumeMiscRenderer::~YumeMiscRenderer()
	{

	}

	void YumeMiscRenderer::Setup()
	{
		mesh_ = new YumeMesh;
		mesh_->Load(gYume->pResourceManager->GetFullPath("Models/Sponza/sponza.obj"));

		sky_.Setup(gYume->pResourceManager->GetFullPath("Models/Skydome/skydome_sphere.obj"));

		renderTarget_ = rhi_->CreateTexture2D();
		renderTarget_->SetSize(gYume->pRHI->GetWidth(),gYume->pRHI->GetHeight(),gYume->pRHI->GetRGBAFloat16FormatNs(),TEXTURE_RENDERTARGET,1,10);

		pp_ = new YumePostProcess(this);
		pp_->Setup();

		//Geometry stuff
		SharedPtr<YumeVertexBuffer> triangleVb(gYume->pRHI->CreateVertexBuffer());

		SimpleVertex v1 ={DirectX::XMFLOAT3(-1.f,-3.f,1.f)};
		SimpleVertex v2 ={DirectX::XMFLOAT3(-1.f,1.f,1.f)};
		SimpleVertex v3 ={DirectX::XMFLOAT3(3.f,1.f,1.f)};

		SimpleVertex vertices[3] ={v1,v2,v3};
		triangleVb->SetShadowed(true);
		triangleVb->SetSize(3,MASK_POSITION);
		triangleVb->SetData(vertices);

		fullscreenTriangle_ = SharedPtr<YumeGeometry>(new YumeGeometry);
		fullscreenTriangle_->SetVertexBuffer(0,triangleVb);
		fullscreenTriangle_->SetDrawRange(TRIANGLE_LIST,0,0,0,3);

		camera_ = new YumeLPVCamera;
		XMMATRIX I = DirectX::XMMatrixIdentity();
		XMFLOAT4X4 sceneWorld;
		XMStoreFloat4x4(&sceneWorld,I);
		mesh_->set_world(sceneWorld);
		UpdateMeshBb(*mesh_);

		lpv_ = new LPVRenderer(this);
		lpv_->Setup();

		DirectX::XMVECTOR d = DirectX::XMVectorSubtract(
			XMLoadFloat3(&bbMax),XMLoadFloat3(&bbMin));

		FLOAT s;
		DirectX::XMStoreFloat(&s,DirectX::XMVector3Length(d));
		s /= 100.f;

		camera_->SetScalers(0.0099f,s);

		dummyTexture_ = gYume->pResourceManager->PrepareResource<YumeTexture2D>("Textures/test/test.jpg");

		overlayPs_ = rhi_->GetShader(PS,"LPV/Overlay");

		lpv_->SetLPVPos(0,bbMax.y,0);
		lpv_->SetInitialLPVPos(0,bbMax.y,0);
	}

	void YumeMiscRenderer::Update(float timeStep)
	{
		if(!gYume->pInput->GetMouseButtonDown(MOUSEB_LEFT))
			camera_->FrameMove(timeStep);

		lpv_->Update(timeStep);
	}

	void YumeMiscRenderer::Render()
	{
		lpv_->Render();

		pp_->Render();
	}

	void YumeMiscRenderer::RenderFullScreenTexture(const IntRect& rect,YumeTexture2D* overlaytexture)
	{
		YumeTexture2D* texture[] ={overlaytexture};
		rhi_->PSBindSRV(10,1,texture);



		YumeShaderVariation* triangle = gYume->pRHI->GetShader(VS,"LPV/fs_triangle");

		rhi_->SetBlendMode(BLEND_PREMULALPHA);
		rhi_->BindSampler(PS,0,1,0); //Standard
		rhi_->SetShaders(triangle,overlayPs_,0);

		rhi_->BindBackbuffer();

		fullscreenTriangle_->Draw(rhi_);
	}

	void YumeMiscRenderer::RenderSky()
	{
		//Sky will set its shaders.
		sky_.Render();
	}

	void YumeMiscRenderer::RenderScene()
	{
		//The shaders must have been setup already
		mesh_->Render();
	}

	void YumeMiscRenderer::SetGIDebug(bool enabled)
	{
		lpv_->SetGIDebug(enabled);
	}

	void YumeMiscRenderer::SetGIScale(float f)
	{
		lpv_->SetGIScale(f);
	}

	void YumeMiscRenderer::SetLPVFluxAmp(float f)
	{
		lpv_->SetLPVFlux(f);
	}

	void YumeMiscRenderer::SetLPVPos(float x,float y,float z)
	{
		lpv_->SetLPVPos(x,y,z);
	}

	void YumeMiscRenderer::SetLightFlux(float f)
	{
		lpv_->SetLightFlux(f);
	}

	void YumeMiscRenderer::SetLPVNumberIterations(int num)
	{
		lpv_->SetNumIterations(num);
	}

	void YumeMiscRenderer::SetPerFrameConstants()
	{
		float totalTime = gYume->pTimer->GetElapsedTime();
		float timeStep = gYume->pTimer->GetTimeStep();

		rhi_->SetShaderParameter("time_delta",timeStep);
		rhi_->SetShaderParameter("time",totalTime);
	}

	void YumeMiscRenderer::UpdateMeshBb(YumeMesh& mesh)
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

	void YumeMiscRenderer::SetCameraParameters(bool shadowPass)
	{
		XMMATRIX view = GetCamera()->GetViewMatrix();
		XMMATRIX proj = GetCamera()->GetProjMatrix();
		XMFLOAT4 cameraPos;

		if(shadowPass)
		{
			XMStoreFloat4(&cameraPos,XMLoadFloat4(&lpv_->dir_light_.position));
			XMMATRIX lightView = XMMatrixLookToLH(XMLoadFloat4(&lpv_->dir_light_.position),XMLoadFloat4(&lpv_->dir_light_.normal),lpv_->dirLightUp);

			view = lightView;
			proj = MakeProjection();
		}
		else
			XMStoreFloat4(&cameraPos,GetCamera()->GetEyePt());

		XMMATRIX vp = view * proj;
		XMMATRIX vpInv = XMMatrixInverse(nullptr,vp);

		gYume->pRHI->SetShaderParameter("vp",vp);
		gYume->pRHI->SetShaderParameter("vp_inv",vpInv);
		gYume->pRHI->SetShaderParameter("camera_pos",cameraPos);
		gYume->pRHI->SetShaderParameter("z_far",zFar);
	}

	
	DirectX::XMMATRIX YumeMiscRenderer::MakeProjection()
	{
		float n = zNear;
		float f = zFar;

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
