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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeGLRendererImpl_h__
#define __YumeGLRendererImpl_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"
#include "Math/YumeMath.h"
#include "Renderer/YumeRendererDefs.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGLRenderer;
	class YumeRenderable;

	/// Cached state of a frame buffer object
	struct FrameBufferObject
	{
		FrameBufferObject():
			fbo_(0),
			depthAttachment_(0),
			readBuffers_(M_MAX_UNSIGNED),
			drawBuffers_(M_MAX_UNSIGNED)
		{
			for(unsigned i = 0; i < MAX_RENDERTARGETS; ++i)
				colorAttachments_[i] = 0;
		}

		/// Frame buffer handle.
		unsigned fbo_;
		/// Bound color attachment textures.
		YumeRenderable* colorAttachments_[MAX_RENDERTARGETS];
		/// Bound depth/stencil attachment.
		YumeRenderable* depthAttachment_;
		/// Read buffer bits.
		unsigned readBuffers_;
		/// Draw buffer bits.
		unsigned drawBuffers_;
	};

	class YumeGLExport YumeGLRendererImpl
	{
		friend class YumeGLRenderer;
	public:
		YumeGLRendererImpl();

		/// Return the GL Context.
		const SDL_GLContext& GetGLContext() { return context_; }

	private:
		/// SDL OpenGL context.
		SDL_GLContext context_;
		/// IOS system framebuffer handle.
		unsigned systemFBO_;
		/// Active texture unit.
		unsigned activeTexture_;
		/// Vertex attributes in use.
		unsigned enabledAttributes_;
		/// Currently bound frame buffer object.
		unsigned boundFBO_;
		/// Currently bound vertex buffer object.
		unsigned boundVBO_;
		/// Currently bound uniform buffer object.
		unsigned boundUBO_;
		/// Current pixel format.
		int pixelFormat_;
		/// Map for FBO's per resolution and format.
		YumeMap<unsigned long long, FrameBufferObject>::type frameBuffers_;
		/// Need FBO commit flag.
		bool fboDirty_;
		/// sRGB write mode flag.
		bool sRGBWrite_;

	};
}


//----------------------------------------------------------------------------
#endif
