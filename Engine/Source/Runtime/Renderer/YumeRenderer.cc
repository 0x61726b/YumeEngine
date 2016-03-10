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

#include "YumeGeometry.h"
#include "YumeMaterial.h"
#include "Scene/YumeOctree.h"

#include "Core/YumeXmlFile.h"

#include "YumeRenderPipeline.h"

#include "Renderer/YumeResourceManager.h"
#include "Engine/YumeEngine.h"
#include "Scene/YumeScene.h"
#include "Math/YumeMatrix3x4.h"

#include "YumeCamera.h"

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

		REGISTER_ENGINE_LISTENER;

		YumeResourceManager* rm_ = YumeEngine3D::Get()->GetResourceManager();

		
		
		
		defaultPipeline_ = YumeAPINew YumeRenderPipeline;
		defaultPipeline_->Load(rm_->PrepareResource<YumeXmlFile>("Shaders/Pipelines/Forward.xml").get());

	}

	void YumeRenderer::QueueRenderable(YumeRenderable* renderTarget)
	{

	}

	void YumeRenderer::SetViewport(int index,SharedPtr<YumeViewport> viewport)
	{
		if(index >= viewports_.size())
			viewports_.resize(index + 1);

		viewports_[index] = viewport;
	}

	void YumeRenderer::Update(float timeStep)
	{
		YumeScene* scene = viewports_[0]->GetScene();
		frame_.timeStep_ = timeStep;
		frame_.camera_ = viewports_[0]->GetCamera();
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

	
		YumeShaderVariation* vs = rhi_->GetShader(VS,"Basic","");
		YumeShaderVariation* ps = rhi_->GetShader(PS,"Basic","");




		Matrix3x4 world = Matrix3x4::IDENTITY;
		world.SetTranslation(Vector3(0,0,0));



		Matrix3x4 view_ = frame_.camera_->GetView();
		Matrix4 viewProj = frame_.camera_->GetProjection() * view_;
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
		rhi_->SetShaderParameter(VSP_VIEWPROJ,viewProj);
		rhi_->SetShaderParameter(PSP_MATDIFFCOLOR,YumeColor(1.0f,1.0f,1.0f,1.0f));


		rhi_->SetVertexBuffers(vbs_,elementMasks_);
		rhi_->SetIndexBuffer(ib_);

		Vector4 depth = Vector4::ZERO;
		depth.w_ = 1 / 1000.0f;
		rhi_->SetShaderParameter(VSP_DEPTHMODE,depth);

		rhi_->Draw(TRIANGLE_LIST,0,36,0,8);
	}

	void YumeRenderer::HandleRenderUpdate(float timeStep)
	{
		Update(timeStep);
	}
}
