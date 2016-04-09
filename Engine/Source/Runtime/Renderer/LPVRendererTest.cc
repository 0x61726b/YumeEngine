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


#include "YumeStaticModel.h"

namespace YumeEngine
{
	LPVRenderer::LPVRenderer()
		: cornell_(0),
		meshVs_(0),
		meshPs_(0),
		world_(Matrix4::IDENTITY),
		view_(Matrix4::IDENTITY),
		proj_(Matrix4::IDENTITY),
		cameraPos_(Vector3(0,0,0))
	{
		rhi_ = gYume->pRHI;
		renderer_ = gYume->pRenderer;
	}

	LPVRenderer::~LPVRenderer()
	{
	}

	void LPVRenderer::Setup()
	{
		YumeStaticModel* cornell = (new YumeStaticModel);
		cornell->SetModel(gYume->pResourceManager->PrepareResource<YumeModel>("Models/Box.mdl"));

		cornell_ = cornell->GetLodGeometry(0,0);

		cameraPos_ = Vector3(0,1,-5);

		view_ = GetView(cameraPos_,Vector3(0,0,0),Vector3::UP,false);
		proj_ = GetProj(0.01f,10000.0f,45,gYume->pRHI->GetWidth() / gYume->pRHI->GetHeight());

		meshVs_ = gYume->pRHI->GetShader(VS,"LPV/Mesh");
		meshPs_ = gYume->pRHI->GetShader(PS,"LPV/Mesh");

		RsmColors_ = gYume->pRHI->CreateTexture2D();
		RsmColors_->SetSize(1024,1024,gYume->pRHI->GetRGBAFloat16FormatNs(),TextureUsage::TEXTURE_RENDERTARGET);


		RsmNormals_ = gYume->pRHI->CreateTexture2D();
		RsmNormals_->SetSize(1024,1024,24,TextureUsage::TEXTURE_RENDERTARGET); //DXGI_FORMAT_R10G10B10A2_UNORM

		RsmDummySpec_ = gYume->pRHI->CreateTexture2D();
		RsmDummySpec_->SetSize(1024,1024,gYume->pRHI->GetRGBAFloat16FormatNs(),TextureUsage::TEXTURE_RENDERTARGET); //DXGI_FORMAT_R10G10B10A2_UNORM

		RsmLinearDepth_ = gYume->pRHI->CreateTexture2D();
		RsmLinearDepth_->SetSize(1024,1024,16,TextureUsage::TEXTURE_RENDERTARGET); //DXGI_FORMAT_R32G32_FLOAT

		RsmDepthStencil_ = gYume->pRHI->CreateTexture2D();
		RsmDepthStencil_->SetSize(1024,1024,39,TextureUsage::TEXTURE_DEPTHSTENCIL); //DXGI_FORMAT_R32_TYPELESS
	}

	void LPVRenderer::SetCameraParameters()
	{
		gYume->pRHI->SetShaderParameter("vp",view_ * proj_);
		gYume->pRHI->SetShaderParameter("vp_inv",(view_ * proj_).Inverse());
		gYume->pRHI->SetShaderParameter("camera_pos",cameraPos_);
	}

	void LPVRenderer::Update()
	{
	}

	void LPVRenderer::Render()
	{
		
		rhi_->SetShaders(meshVs_,meshPs_);

		SetCameraParameters();

		rhi_->SetRenderTarget(0,RsmColors_);
		rhi_->SetRenderTarget(1,RsmNormals_);
		rhi_->SetRenderTarget(2,RsmLinearDepth_);
		rhi_->SetRenderTarget(3,RsmDummySpec_);

		rhi_->SetDepthStencil(RsmDepthStencil_);

		rhi_->SetViewport(IntRect(0,0,1024,1024));

		

		cornell_->Draw(gYume->pRHI);
	}

	Matrix4 LPVRenderer::GetView(const Vector3& vEye,const Vector3& vAt,const Vector3& vUp,bool bRightHanded)
	{
		Vector3 vForward,vUpNorm,vSide;

		vForward = (bRightHanded) ? vEye - vAt : vAt - vEye;

		vForward = vForward.Normalized();
		vUpNorm = vUp.Normalized();
		vSide = vUpNorm.CrossProduct(vForward).Normalized();
		vUpNorm = vForward.CrossProduct(vSide).Normalized();

		Matrix4 viewMatrix;
		// first column
		viewMatrix.m00_ = vSide.x_;
		viewMatrix.m01_ = vSide.y_;
		viewMatrix.m02_ = vSide.z_;
		viewMatrix.m03_ = 0.0f;

		// second column
		viewMatrix.m10_ = vUpNorm.x_;
		viewMatrix.m11_ = vUpNorm.y_;
		viewMatrix.m12_ = vUpNorm.z_;
		viewMatrix.m13_ = 0.0f;

		// third column
		viewMatrix.m20_ = vForward.x_;
		viewMatrix.m21_ = vForward.y_;
		viewMatrix.m22_ = vForward.z_;
		viewMatrix.m23_ = 0;

		// fourth column
		viewMatrix.m30_ = 0.0f;
		viewMatrix.m31_ = 0.0f;
		viewMatrix.m32_ = 0.0f;
		viewMatrix.m33_ = 1.0f;

		Matrix4 t;
		t.SetTranslation(Vector3(-vEye.x_,-vEye.y_,-vEye.z_));

		viewMatrix = viewMatrix * t;

		return viewMatrix;
	}

	Matrix4 LPVRenderer::GetProj(float nearr,float farr,float fieldOfView,float aspectRatio)
	{
		Matrix4 projectionMatrix;

		float right = nearr * tanf(M_DEGTORAD * (fieldOfView)* 0.5f);
		float top = right / aspectRatio;

		// first column
		projectionMatrix.m00_ = nearr / right;
		projectionMatrix.m01_ = 0.0f;
		projectionMatrix.m02_ = 0.0f;
		projectionMatrix.m03_ = 0.0f;

		// second column
		projectionMatrix.m10_ = 0.0f;
		projectionMatrix.m11_ = nearr / top;
		projectionMatrix.m12_ = 0.0f;
		projectionMatrix.m13_ = 0.0f;

		// third column
		projectionMatrix.m20_ = 0;
		projectionMatrix.m21_ = 0;
		projectionMatrix.m22_ = -(farr + nearr) / (farr - nearr);
		projectionMatrix.m23_ = -1;

		// fourth column
		projectionMatrix.m30_ = 0.0f;
		projectionMatrix.m31_ = 0.0f;
		projectionMatrix.m32_ = -(2 * farr * nearr) / (farr - nearr);
		projectionMatrix.m33_ = 0.0f;

		return projectionMatrix;
	}
}
