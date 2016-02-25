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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeGraphicsInterface_h__
#define __YumeGraphicsInterface_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeRendererDefs.h"

#include "Math/YumeVector4.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeGpuResource;
	class YumeRendererImpl;

	class YumeAPIExport YumeRenderer : public RenderObjAlloc
	{
	public:
		YumeRenderer();

		virtual ~YumeRenderer();

		virtual bool SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
			int multiSample) = 0;

		virtual bool BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Clear(unsigned flags,const Vector4& color = Vector4(0.0f,1.0f,0.0f,0.0f),float depth = 1.0f,unsigned stencil = 0) = 0;

		virtual void ResetRenderTargets() = 0;
		virtual void SetViewport(const Vector4&) = 0;

		virtual Vector2 GetRenderTargetDimensions() const = 0;

		virtual void SetFlushGPU(bool flushGpu) = 0;
		virtual void CreateRendererCapabilities() = 0;
		virtual YumeVector<int>::type GetMultiSampleLevels() const = 0;

		virtual void Close() = 0;
		virtual void AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless) = 0;
		virtual void Maximize() = 0;
		virtual void SetWindowPos(const Vector2& pos) = 0;
		virtual void SetWindowTitle(const YumeString& string) = 0;

		virtual void AddGpuResource(YumeGpuResource* object) = 0;
		virtual void RemoveGpuResource(YumeGpuResource* object) = 0;
	};
}


//----------------------------------------------------------------------------
#endif
