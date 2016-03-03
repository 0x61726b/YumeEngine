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

	class YumeAPIExport YumeRenderable
	{
		friend class Texture2D;
		friend class TextureCube;

	public:
		/// Construct with parent texture.
		YumeRenderable(YumeTexture* parentTexture);
		/// Destruct.
		~YumeRenderable();

		/// Set number of viewports.
		void SetNumViewports(unsigned num);
		/// Set viewport.
		void SetViewport(unsigned index,YumeViewport* viewport);
		/// Set viewport update mode. Default is to update when visible.
		void SetUpdateMode(RenderSurfaceUpdateMode mode);
		/// Set linked color rendertarget.
		void SetLinkedRenderTarget(YumeRenderable* renderTarget);
		/// Set linked depth-stencil surface.
		void SetLinkedDepthStencil(YumeRenderable* depthStencil);
		/// Queue manual update of the viewport(s).
		void QueueUpdate();
		/// Release surface.
		void Release();

		/// Return parent texture.
		YumeTexture* GetParentTexture() const { return parentTexture_; }

		/// Return Direct3D rendertarget or depth-stencil view.
		void* GetRenderTargetView() const { return renderTargetView_; }

		/// Return Direct3D read-only depth-stencil view. May be null if not applicable
		void* GetReadOnlyView() const { return readOnlyView_; }

		/// Return width.
		int GetWidth() const;
		/// Return height.
		int GetHeight() const;
		/// Return usage.
		TextureUsage GetUsage() const;

		/// Return number of viewports.
		unsigned GetNumViewports() const { return viewports_.size(); }

		/// Return viewport by index.
		YumeViewport* GetViewport(unsigned index) const;

		/// Return viewport update mode.
		RenderSurfaceUpdateMode GetUpdateMode() const { return updateMode_; }

		/// Return linked color rendertarget.
		YumeRenderable* GetLinkedRenderTarget() const { return linkedRenderTarget_; }

		/// Return linked depth-stencil surface.
		YumeRenderable* GetLinkedDepthStencil() const { return linkedDepthStencil_; }

		/// Return whether manual update queued. Called internally.
		bool IsUpdateQueued() const { return updateQueued_; }
		/// Reset update queued flag. Called internally.
		void ResetUpdateQueued();

	public:
		/// Parent texture.
		YumeTexture* parentTexture_;
		/// Direct3D rendertarget or depth-stencil view.
		void* renderTargetView_;
		/// Direct3D read-only depth-stencil view. Present only on depth-stencil surfaces.
		void* readOnlyView_;
		/// Viewports.
		YumeVector<SharedPtr<YumeViewport> >::type viewports_;
		/// Linked color buffer.
		YumeRenderable* linkedRenderTarget_;
		/// Linked depth buffer.
		YumeRenderable* linkedDepthStencil_;
		/// Update mode for viewports.
		RenderSurfaceUpdateMode updateMode_;
		/// Update queued flag.
		bool updateQueued_;
	};
}


//----------------------------------------------------------------------------
#endif
