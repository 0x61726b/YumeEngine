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
#include "Renderer/YumeRHI.h"
#include "YumeRenderer.h"

#include "YumeTexture2D.h"
#include "YumeVertexBuffer.h"
#include "YumeIndexBuffer.h"
#include "YumeInputLayout.h"

#include "Math/YumeMatrix3x4.h"

namespace YumeEngine
{
	static const float dummyVertices[] =
	{
		1,1,-1,0,
		-1,1,-1,0,
		-1,1,1,0,
		1,1,1,0,
		1,-1,-1,0,
		-1,-1,-1,0,
		-1,-1,1,0,
		1,-1,1,0
	};

	static const unsigned short dummyIndices[] =
	{
		0,1,2,
		0,2,3,
		0,4,5,
		0,5,1,
		1,5,6,
		1,6,2,
		2,6,7,
		2,7,3,
		3,7,4,
		3,4,0,
		4,7,6,
		4,6,5
	};

	YumeRenderer::YumeRenderer(YumeRHI* rhi)
		:textureQuality_(QUALITY_HIGH)
	{
		rhi_ = rhi;
	}

	YumeRenderer::~YumeRenderer()
	{
	}

	void YumeRenderer::Initialize()
	{
		elementMasks_.push_back(MASK_POSITION | MASK_COLOR);

		YumeVertexBuffer* vb_ = rhi_->CreateVertexBuffer();
		vb_->SetShadowed(true);
		vb_->SetSize(8,MASK_POSITION | MASK_COLOR);
		vb_->SetData(dummyVertices);

		vbs_.push_back(vb_);



		ib_ = rhi_->CreateIndexBuffer();
		ib_->SetShadowed(true);
		ib_->SetSize(36,false);

		ib_->SetData(dummyIndices);



	}

	void YumeRenderer::QueueRenderable(YumeRenderable* renderTarget)
	{

	}


	void YumeRenderer::Render()
	{
		rhi_->SetBlendMode(BLEND_REPLACE);
		rhi_->SetColorWrite(true);
		rhi_->SetDepthWrite(true);
		rhi_->SetScissorTest(false);
		rhi_->SetStencilTest(false);
		rhi_->ResetRenderTargets();
		rhi_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);

		float nearClip = 0.1f;
		float farClip_ = 1000.0f;
		float aspectRatio_ = 16.0f/9.0f;

		YumeShaderVariation* vs = rhi_->GetShader(VS,"Basic","");
		YumeShaderVariation* ps = rhi_->GetShader(PS,"Basic","");



		//Matrix4 projection_ = Matrix4::ZERO;

		//float h = (1.0f / tanf(60.0f * M_PI / 180.0f * 0.5f)) * 1;
		//float w = h / aspectRatio_;
		//float q,r;


		//q = farClip_ / (farClip_ - nearClip);
		//r = -q * nearClip;

		//Vector2 projectionOffset_ = Vector2::ZERO;

		//projection_.m00_ = w;
		//projection_.m02_ = projectionOffset_.x_ * 2.0f;
		//projection_.m11_ = h;
		//projection_.m12_ = projectionOffset_.y_ * 2.0f;
		//projection_.m22_ = q;
		//projection_.m23_ = r;
		//projection_.m32_ = 1.0f;

		Vector2 invScreenSize(1.0f / (float)1600,1.0f / (float)720);
		Vector2 scale(2.0f * invScreenSize.x_,-2.0f * invScreenSize.y_);
		Vector2 offset(-1.0f,1.0f);

		Matrix4 projection(Matrix4::IDENTITY);
		projection.m00_ = scale.x_;
		projection.m03_ = offset.x_;
		projection.m11_ = scale.y_;
		projection.m13_ = offset.y_;
		projection.m22_ = 1.0f;
		projection.m23_ = 0.0f;
		projection.m33_ = 1.0f;

		Matrix3x4 world = Matrix3x4::IDENTITY;
		world.SetTranslation(Vector3(0,0,0));

		Vector3 cameraPos = Vector3(0,0,5);
		Vector3 lookAt = Vector3(0,0,0);
		Quaternion lookAtRot;
		lookAtRot.FromLookRotation(lookAt - cameraPos);


		Matrix3x4 view_ = Matrix3x4(cameraPos,lookAtRot,1);
		Matrix4 viewProj = projection * view_;
		rhi_->SetShaders(vs,ps);

		rhi_->ClearParameterSources();

		rhi_->SetBlendMode(BLEND_REPLACE);
		rhi_->SetColorWrite(true);
		rhi_->SetCullMode(CULL_NONE);
		rhi_->SetDepthWrite(true);
		rhi_->SetScissorTest(false);
		rhi_->SetStencilTest(false);
		rhi_->SetShaderParameter(VSP_MODEL,world);
		rhi_->SetShaderParameter(VSP_VIEW,view_);
		rhi_->SetShaderParameter(VSP_VIEWINV,view_.Inverse());
		rhi_->SetShaderParameter(VSP_VIEWPROJ,projection * view_);
		rhi_->SetShaderParameter(PSP_MATDIFFCOLOR,YumeColor(1.0f,1.0f,1.0f,1.0f));
		rhi_->SetShaderParameter(VSP_CAMERAPOS,Vector3(0,0,-10));
		rhi_->SetShaderParameter(VSP_CAMERAROT,Matrix3::IDENTITY);
		rhi_->SetShaderParameter(VSP_NEARCLIP,0.0f);
		rhi_->SetShaderParameter(VSP_FARCLIP,1000.0f);
		rhi_->SetShaderParameter(PSP_NEARCLIP,0.0f);
		rhi_->SetShaderParameter(PSP_FARCLIP,1000.0f);


		rhi_->SetVertexBuffers(vbs_,elementMasks_);
		rhi_->SetIndexBuffer(ib_);

		Vector4 depth = Vector4::ZERO;
		depth.w_ = 1 / 1000.0f;
		rhi_->SetShaderParameter(VSP_DEPTHMODE,depth);

		rhi_->Draw(TRIANGLE_LIST,0,36,0,8);
	}
}
