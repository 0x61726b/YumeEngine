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
#ifndef __YumeRenderable_h__
#define __YumeRenderable_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeTexture.h"
#include "YumeViewport.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class YumeAPIExport YumeRenderable : public YumeBase
	{
		friend class YumeTexture2D;
		friend class YumeTextureCube;

	public:
		YumeRenderable(YumeTexture* parentTexture);
		~YumeRenderable();

		void SetNumViewports(unsigned num);
		void SetViewport(unsigned index,YumeViewport* viewport);
		void SetUpdateMode(RenderSurfaceUpdateMode mode);
		void SetLinkedRenderTarget(YumeRenderable* renderTarget);
		void SetLinkedDepthStencil(YumeRenderable* depthStencil);
		void QueueUpdate();
		virtual void Release() = 0;

		YumeTexture* GetParentTexture() const { return parentTexture_; }

		void* GetRenderTargetView() const { return renderTargetView_; }

		void* GetReadOnlyView() const { return readOnlyView_; }

		int GetWidth() const;
		int GetHeight() const;
		TextureUsage GetUsage() const;

		unsigned GetNumViewports() const { return viewports_.size(); }

		YumeViewport* GetViewport(unsigned index) const;
		RenderSurfaceUpdateMode GetUpdateMode() const { return updateMode_; }

		YumeRenderable* GetLinkedRenderTarget() const { return linkedRenderTarget_; }
		YumeRenderable* GetLinkedDepthStencil() const { return linkedDepthStencil_; }

		bool IsUpdateQueued() const { return updateQueued_; }
		void ResetUpdateQueued();

	public:
		YumeTexture* parentTexture_;
		void* renderTargetView_;
		void* readOnlyView_;
		YumeVector<SharedPtr<YumeViewport> >::type viewports_;
		WeakPtr<YumeRenderable> linkedRenderTarget_;
		WeakPtr<YumeRenderable> linkedDepthStencil_;
		RenderSurfaceUpdateMode updateMode_;
		bool updateQueued_;
	};
}


//----------------------------------------------------------------------------
#endif
