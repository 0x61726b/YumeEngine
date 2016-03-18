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
#ifndef __YumeGLRenderable_h__
#define __YumeGLRenderable_h__
//----------------------------------------------------------------------------
#include "YumeGLRequired.h"
#include "Renderer/YumeRendererDefs.h"
#include "Renderer/YumeViewport.h"
#include "Renderer/YumeRenderable.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeTexture;

	class YumeGLExport YumeGLRenderable : public YumeRenderable
	{
	public:
		YumeGLRenderable(YumeTexture* parentTexture);
		~YumeGLRenderable();

		bool CreateRenderBuffer(unsigned width,unsigned height,unsigned format);
		void OnDeviceLost();
		virtual void Release();

		unsigned GetRenderBuffer() const { return renderBuffer_; }
		unsigned GetTarget() const { return target_; }

		void SetTarget(unsigned target);
	private:
		unsigned target_;
		unsigned renderBuffer_;
	};
}


//----------------------------------------------------------------------------
#endif
