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
#ifndef __YumeNullRenderer_h__
#define __YumeNullRenderer_h__
//----------------------------------------------------------------------------
#include "Core/YumeRequired.h"
#include "Renderer/YumeRenderer.h"

#	if YUME_PLATFORM == YUME_PLATFORM_WIN32
#		if defined(BUILDING_YUME_NULL)
#			define YumeNullExport __declspec( dllexport )
#		else
#			define YumeNullExport  __declspec( dllimport )
#		endif
#	else
#	define YumeNullExport 	
#	endif

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeNullExport YumeNullRenderer : public YumeRenderer
	{
	public:
		YumeNullRenderer();
		~YumeNullRenderer();


		bool SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
			int multiSample);

		bool BeginFrame();
		void EndFrame();
		void Clear(unsigned flags, const Vector4& color = Vector4(0.0f, 1.0f, 0.0f, 0.0f), float depth = 1.0f, unsigned stencil = 0);

		bool CreateD3D11Device(int width,int height,int multisample);
		bool UpdateSwapchain(int width,int height);

		void ResetRenderTargets();
		void SetViewport(const Vector4&);

		Vector2 GetRenderTargetDimensions() const;

		void SetFlushGPU(bool flushGpu);
		void CreateRendererCapabilities();
		YumeVector<int>::type GetMultiSampleLevels() const;

		//Window ops
		void Close();
		void AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless);
		void Maximize();
		void SetWindowPos(const Vector2& pos);
		void SetWindowTitle(const YumeString& string);

		void AddGpuResource(YumeGpuResource* object);
		void RemoveGpuResource(YumeGpuResource* object);
	};
}


//----------------------------------------------------------------------------
#endif
