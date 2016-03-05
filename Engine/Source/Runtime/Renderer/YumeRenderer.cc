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
		-1,1,0,
		1,1,0,
		1,-1,0,
		-1,-1,0,
	};

	static const unsigned short dummyIndices[] =
	{
		0,1,2,
		2,3,0,
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

		YumeShaderVariation* vs = rhi_->GetShader(VS,"Basic","VERTEXCOLOR");
		YumeShaderVariation* ps = rhi_->GetShader(PS,"Basic","VERTEXCOLOR");


		YumeVertexBuffer* vb = rhi_->CreateVertexBuffer();
		vb->SetSize(4,MASK_POSITION | MASK_COLOR);
		vb->SetData(dummyVertices);
		vb->SetShadowed(true);

		YumeIndexBuffer* dlib = rhi_->CreateIndexBuffer();
		dlib->SetShadowed(true);
		dlib->SetSize(6,false);
		dlib->SetData(dummyIndices);

		Matrix3x4 view_ = Matrix3x4(Vector3(1,1,1),Quaternion(0,0,0,1),1.0f).Inverse();
		Matrix3x4 projection_ = Matrix3x4::IDENTITY;



		rhi_->SetBlendMode(BLEND_REPLACE);
		rhi_->SetColorWrite(true);
		rhi_->SetCullMode(CULL_NONE);
		rhi_->SetDepthWrite(true);
		rhi_->SetScissorTest(false);
		rhi_->SetStencilTest(false);
		rhi_->SetShaders(vs,ps);
		rhi_->SetShaderParameter(VSP_MODEL,Matrix3x4::IDENTITY);
		rhi_->SetShaderParameter(VSP_VIEW,view_);
		rhi_->SetShaderParameter(VSP_VIEWINV,view_.Inverse());
		rhi_->SetShaderParameter(VSP_VIEWPROJ,projection_ * view_);
		rhi_->SetShaderParameter(PSP_MATDIFFCOLOR,YumeColor(1.0f,1.0f,1.0f,1.0f));

		rhi_->SetShaderParameter(VSP_CAMERAPOS,Vector3(0,0,10));
		rhi_->SetShaderParameter(VSP_CAMERAROT,Matrix3::IDENTITY);
		rhi_->SetShaderParameter(PSP_CAMERAPOS,Vector3(0,0,-10));
		rhi_->SetShaderParameter(VSP_NEARCLIP,0.0f);
		rhi_->SetShaderParameter(VSP_FARCLIP,1000.0f);
		rhi_->SetShaderParameter(PSP_NEARCLIP,0.0f);
		rhi_->SetShaderParameter(PSP_FARCLIP,1000.0f);

		rhi_->SetVertexBuffer(vb);
		rhi_->SetIndexBuffer(dlib);

		Vector4 depth = Vector4::ZERO;
		depth.w = 1 / 1000.0f;
		rhi_->SetShaderParameter(VSP_DEPTHMODE,depth);

	}

	void YumeRenderer::QueueRenderable(YumeRenderable* renderTarget)
	{

	}


	void YumeRenderer::Render()
	{
		rhi_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);

		rhi_->Draw(TRIANGLE_LIST,0,6,0,4);
	}
}
