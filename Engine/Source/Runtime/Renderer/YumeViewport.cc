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
#include "YumeViewport.h"
#include "YumeCamera.h"


namespace YumeEngine
{

	YumeViewport::YumeViewport():
		rect_(IntRect::ZERO),
		drawDebug_(true)
	{
		SetRenderPath((YumeRenderPath*)0);
	}

	YumeViewport::YumeViewport(YumeScene* scene,YumeCamera* camera,YumeRenderPath* renderPath):
		camera_(camera),
		scene_(scene),
		rect_(IntRect::ZERO),
		drawDebug_(true)
	{
		SetRenderPath(renderPath);
	}

	YumeViewport::YumeViewport(YumeCamera* camera,const IntRect& rect,YumeRenderPath* renderPath):
		camera_(camera),
		rect_(rect),
		drawDebug_(true)
	{
		SetRenderPath(renderPath);
	}
	YumeViewport::~YumeViewport()
	{
	}

	void YumeViewport::SetRenderPath(YumeRenderPath*)
	{
	}

	YumeCamera* YumeViewport::GetCamera() const
	{
		return camera_;
	}

	YumeScene* YumeViewport::GetScene() const
	{
		return scene_;
	}

	void YumeViewport::SetCamera(YumeCamera* camera)
	{

	}
}
