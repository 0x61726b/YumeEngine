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
#include "YumeRenderable.h"
#include "YumeRHI.h"
#include "YumeTexture.h"

#include "Engine/YumeEngine.h"

namespace YumeEngine
{

	YumeRenderable::YumeRenderable(YumeTexture* parentTexture):
		parentTexture_(parentTexture),
		renderTargetView_(0),
		readOnlyView_(0),
		updateMode_(SURFACE_UPDATEVISIBLE),
		updateQueued_(false)
	{
	}

	YumeRenderable::~YumeRenderable()
	{
		
	}

	void YumeRenderable::SetNumViewports(unsigned num)
	{
		viewports_.resize(num);
	}

	void YumeRenderable::SetViewport(unsigned index,YumeViewport* viewport)
	{
		if(index >= viewports_.size())
			viewports_.resize(index + 1);

		viewports_[index] = viewport;
	}

	void YumeRenderable::SetUpdateMode(RenderSurfaceUpdateMode mode)
	{
		updateMode_ = mode;
	}

	void YumeRenderable::SetLinkedRenderTarget(YumeRenderable* renderTarget)
	{
		if(renderTarget != this)
			linkedRenderTarget_ = renderTarget;
	}

	void YumeRenderable::SetLinkedDepthStencil(YumeRenderable* depthStencil)
	{
		if(depthStencil != this)
			linkedDepthStencil_ = depthStencil;
	}

	void YumeRenderable::QueueUpdate()
	{
		updateQueued_ = true;
	}

	void YumeRenderable::ResetUpdateQueued()
	{
		updateQueued_ = false;
	}

	int YumeRenderable::GetWidth() const
	{
		return parentTexture_->GetWidth();
	}

	int YumeRenderable::GetHeight() const
	{
		return parentTexture_->GetHeight();
	}

	TextureUsage YumeRenderable::GetUsage() const
	{
		return parentTexture_->GetUsage();
	}

	YumeViewport* YumeRenderable::GetViewport(unsigned index) const
	{
		return index < viewports_.size() ? viewports_[index] : (YumeViewport*)0;
	}
}
