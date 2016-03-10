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
#include "Core/YumeXmlFile.h"
#include "Renderer/YumeRenderPipeline.h"
#include "Renderer/YumeRenderer.h"

#include "YumeRHI.h"
#include "Engine/YumeEngine.h"


namespace YumeEngine
{

	YumeViewport::YumeViewport():
		rect_(IntRect::ZERO),
		drawDebug_(true)
	{
		SetRenderPath((YumeRenderPipeline*)0);
	}

	YumeViewport::YumeViewport(YumeScene* scene,YumeCamera* camera,YumeRenderPipeline* renderPath):
		camera_(camera),
		scene_(scene),
		rect_(IntRect::ZERO),
		drawDebug_(true)
	{
		SetRenderPath(renderPath);
	}

	YumeViewport::YumeViewport(YumeCamera* camera,const IntRect& rect,YumeRenderPipeline* renderPath):
		camera_(camera),
		rect_(rect),
		drawDebug_(true)
	{
		SetRenderPath(renderPath);
	}
	YumeViewport::~YumeViewport()
	{
	}

	void YumeViewport::SetRenderPath(YumeRenderPipeline* path)
	{
		if(path)
			renderPath_ = SharedPtr<YumeRenderPipeline>(path);
		else
		{
			SharedPtr<YumeRenderer> renderer= YumeEngine3D::Get()->GetRenderLogic();
			if(renderer)
				renderPath_ = SharedPtr<YumeRenderPipeline>(renderer->GetDefaultPipeline());
		}
	}

	void YumeViewport::SetRenderPath(YumeXmlFile* file)
	{
		SharedPtr<YumeRenderPipeline> newRenderPath(new YumeRenderPipeline());
		if(newRenderPath->Load(file))
			renderPath_ = newRenderPath;
	}

	void YumeViewport::SetCamera(YumeCamera* camera)
	{
		camera_ = camera;
	}


	void YumeViewport::SetScene(YumeScene* scene)
	{
		scene_ = scene;
	}


	void YumeViewport::SetCullCamera(YumeCamera* camera)
	{
		camera_ = camera;
	}


	void YumeViewport::SetDrawDebug(bool enable)
	{
		drawDebug_ = enable;
	}

	YumeCamera* YumeViewport::GetCamera() const
	{
		return camera_;
	}

	YumeScene* YumeViewport::GetScene() const
	{
		return scene_;
	}


	YumeCamera* YumeViewport::GetCullCamera() const
	{
		return cullCamera_;
	}


	YumeRenderPipeline* YumeViewport::GetRenderPath() const
	{
		return renderPath_.get();
	}


	Ray YumeViewport::GetScreenRay(int x,int y) const
	{
		if(!camera_)
			return Ray();

		float screenX;
		float screenY;

		if(rect_ == IntRect::ZERO)
		{
			YumeRHI* graphics = YumeEngine3D::Get()->GetRenderer();
			screenX = (float)x / (float)graphics->GetWidth();
			screenY = (float)y / (float)graphics->GetHeight();
		}
		else
		{
			screenX = float(x - rect_.left_) / (float)rect_.Width();
			screenY = float(y - rect_.top_) / (float)rect_.Height();
		}

		return camera_->GetScreenRay(screenX,screenY);
	}

	IntVector2 YumeViewport::WorldToScreenPoint(const Vector3& worldPos) const
	{
		if(!camera_)
			return IntVector2::ZERO;

		Vector2 screenPoint = camera_->WorldToScreenPoint(worldPos);

		int x;
		int y;
		if(rect_ == IntRect::ZERO)
		{
			YumeRHI* graphics = YumeEngine3D::Get()->GetRenderer();
			x = (int)(screenPoint.x_ * graphics->GetWidth());
			y = (int)(screenPoint.y_ * graphics->GetHeight());
		}
		else
		{
			x = (int)(rect_.left_ + screenPoint.x_ * rect_.Width());
			y = (int)(rect_.top_ + screenPoint.y_ * rect_.Height());
		}

		return IntVector2(x,y);
	}

	Vector3 YumeViewport::ScreenToWorldPoint(int x,int y,float depth) const
	{
		if(!camera_)
			return Vector3::ZERO;

		float screenX;
		float screenY;

		if(rect_ == IntRect::ZERO)
		{
			YumeRHI* graphics = YumeEngine3D::Get()->GetRenderer();
			screenX = (float)x / (float)graphics->GetWidth();
			screenY = (float)y / (float)graphics->GetHeight();
		}
		else
		{
			screenX = float(x - rect_.left_) / (float)rect_.Width();
			screenY = float(y - rect_.top_) / (float)rect_.Height();
		}

		return camera_->ScreenToWorldPoint(Vector3(screenX,screenY,depth));
	}

	void YumeViewport::AllocateView()
	{
	}
}
