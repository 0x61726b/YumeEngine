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
#include "YumeHeaders.h"
#include "YumeRequired.h"
#include "YumeNullRenderer.h"

#include "Math/YumeVector2.h"

#include "Math/YumeMath.h"

#include "Logging/logging.h"

#include "Engine/YumeEngine.h"


namespace YumeEngine
{
	extern "C" void YumeNullExport LoadModule(YumeEngine3D* engine) throw()
	{
		YumeRHI* graphics_ = new YumeNullRenderer;
		engine->SetRenderer(graphics_);
	}
	//---------------------------------------------------------------------	
	extern "C" void YumeNullExport UnloadModule(YumeEngine3D* engine) throw()
	{
		YumeRHI* graphics_ = engine->GetRenderer();
		delete graphics_;
	}
	//---------------------------------------------------------------------
	YumeNullRenderer::YumeNullRenderer()
	{
	}

	YumeNullRenderer::~YumeNullRenderer()
	{
		
	}

	bool YumeNullRenderer::BeginFrame()
	{
		return true;
	}

	void YumeNullRenderer::EndFrame()
	{


	}

	void YumeNullRenderer::Clear(unsigned flags,const Vector4& color,float depth,unsigned stencil)
	{
		
	}

	void YumeNullRenderer::CreateRendererCapabilities()
	{
		
	}

	void YumeNullRenderer::ResetRenderTargets()
	{
		
	}

	void YumeNullRenderer::SetViewport(const IntRect& rect)
	{
		
	}

	Vector2 YumeNullRenderer::GetRenderTargetDimensions() const
	{
		return Vector2(0,0);
	}


	YumeVector<int>::type YumeNullRenderer::GetMultiSampleLevels() const
	{
		YumeVector<int>::type levelsVector;
		return levelsVector;
	}


	void YumeNullRenderer::SetFlushGPU(bool flushGpu_)
	{
		
	}

	void YumeNullRenderer::SetWindowPos(const Vector2& pos)
	{

	}
	void YumeNullRenderer::SetWindowTitle(const YumeString& title)
	{

	}

	void YumeNullRenderer::AdjustWindow(int& newWidth,int& newHeight,bool& newFullscreen,bool& newBorderless)
	{

	}

	void YumeNullRenderer::Close()
	{

	}

	bool YumeNullRenderer::SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
		int multiSample)
	{
		return true;
	}

	
	void YumeNullRenderer::Maximize()
	{

	}

	void YumeNullRenderer::AddGpuResource(YumeGpuResource* gpuRes)
	{

	}

	void YumeNullRenderer::RemoveGpuResource(YumeGpuResource* gpuRes)
	{

	}
}
