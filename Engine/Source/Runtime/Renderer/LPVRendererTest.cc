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
		proj_(Matrix4::IDENTITY)
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
		cornell->SetModel( gYume->pResourceManager->PrepareResource<YumeModel>("Models/Plane.mdl") );

		cornell_ = cornell->GetLodGeometry(0,0);
	}

	void LPVRenderer::SetCameraParameters()
	{

	}

	void LPVRenderer::Update()
	{
	}

	void LPVRenderer::Render()
	{
		rhi_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);
	}
}
