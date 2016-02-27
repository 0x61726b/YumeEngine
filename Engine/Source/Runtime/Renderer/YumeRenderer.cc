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
#include "YumeHeaders.h"

#include "YumeImage.h"

#include "YumeRenderer.h"
#include "YumeGpuResource.h"


namespace YumeEngine
{
	YumeRenderer::YumeRenderer():
		windowIcon_(0),
		window_(0)
	{

	}
	YumeRenderer::~YumeRenderer()
	{
		
	}

	bool YumeRenderer::SetGraphicsMode(int width,int height,bool fullscreen,bool borderless,bool resizable,bool vsync,bool tripleBuffer,
			int multiSample)
	{
		CreateWindowIcon();

		return true;
	}
	void YumeRenderer::SetWindowIcon(YumeImage* image)
	{
		windowIcon_ = image;

		if(window_)
			CreateWindowIcon();
	}

	void YumeRenderer::CreateWindowIcon()
	{
		if(windowIcon_)
		{
			SDL_Surface* surface = windowIcon_->GetSDLSurface();
			if(surface)
			{
				SDL_SetWindowIcon(window_,surface);
				SDL_FreeSurface(surface);
			}
		}
	}
}
