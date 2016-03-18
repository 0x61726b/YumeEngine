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
#include "YumeGLRenderable.h"
#include "YumeGLRenderer.h"
#include "Renderer/YumeTexture.h"

#include "Engine/YumeEngine.h"

#ifdef GL_ES_VERSION_2_0
#define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
#define glGenRenderbuffersEXT glGenRenderbuffers
#define glBindRenderbufferEXT glBindRenderbuffer
#define glRenderbufferStorageEXT glRenderbufferStorage
#define glDeleteRenderbuffersEXT glDeleteRenderbuffers
#endif



namespace YumeEngine
{

	YumeGLRenderable::YumeGLRenderable(YumeTexture* parentTexture):
		YumeRenderable(parentTexture),
		target_(GL_TEXTURE_2D),
		renderBuffer_(0)
	{
	}

	YumeGLRenderable::~YumeGLRenderable()
	{
		Release();
	}

	bool YumeGLRenderable::CreateRenderBuffer(unsigned width,unsigned height,unsigned format)
	{
		Release();

		glGenRenderbuffersEXT(1,&renderBuffer_);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,renderBuffer_);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,format,width,height);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,0);
		return true;
	}

	void YumeGLRenderable::OnDeviceLost()
	{
		YumeRHI* graphics = gYume->pRHI;

		for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
		{
			if(graphics->GetRenderTarget(i) == this)
				graphics->ResetRenderTarget(i);
		}

		if(graphics->GetDepthStencil() == this)
			graphics->ResetDepthStencil();

		// Clean up also from non-active FBOs
		static_cast<YumeGLRenderer*>(graphics)->CleanupRenderable(this);

		renderBuffer_ = 0;
	}

	void YumeGLRenderable::Release()
	{
		YumeRHI* graphics = gYume->pRHI;
		if(!graphics)
			return;

		if(!graphics->IsDeviceLost())
		{
			for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
			{
				if(graphics->GetRenderTarget(i) == this)
					graphics->ResetRenderTarget(i);
			}

			if(graphics->GetDepthStencil() == this)
				graphics->ResetDepthStencil();

			// Clean up also from non-active FBOs
			static_cast<YumeGLRenderer*>(graphics)->CleanupRenderable(this);

			if(renderBuffer_)
				glDeleteRenderbuffersEXT(1,&renderBuffer_);
		}

		renderBuffer_ = 0;
	}

	void YumeGLRenderable::SetTarget(unsigned target)
	{
		target_ = target;
	}
}
