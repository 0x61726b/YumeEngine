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
		
	}

	void YumeRenderer::QueueRenderable(YumeRenderable* renderTarget)
	{

	}


	void YumeRenderer::Render()
	{
		rhi_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL);
	}
}
