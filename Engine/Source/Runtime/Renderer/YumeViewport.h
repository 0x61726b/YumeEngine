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
#ifndef __YumeViewport_h__
#define __YumeViewport_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#include "Math/YumeRay.h"
#include "Math/YumeRect.h"
#include "Math/YumeVector2.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeCamera;
	class YumeScene;
	class YumeXmlFile;
	class YumeRenderPipeline;
	class YumeRenderView;


	
	class YumeAPIExport YumeViewport : public YumeBase
	{
	public:
		YumeViewport();
		YumeViewport(YumeScene* scene, YumeCamera* camera, YumeRenderPipeline* renderPath = 0);
		YumeViewport(YumeScene* scene, YumeCamera* camera, const IntRect& rect, YumeRenderPipeline* renderPath = 0);
		~YumeViewport();

		void SetCamera(YumeCamera* camera);
		void SetScene(YumeScene* scene);
		void SetRect(const IntRect& rect);
		void SetRenderPath(YumeRenderPipeline* path);
		void SetRenderPath(YumeXmlFile* file);
		void SetDrawDebug(bool enable);
		void SetCullCamera(YumeCamera* camera);

		YumeCamera* GetCamera() const;
		YumeScene* GetScene() const;
		const IntRect& GetRect() const { return rect_; }
		YumeRenderPipeline* GetRenderPath() const;
		bool GetDrawDebug() const { return drawDebug_; }
		SharedPtr<YumeRenderView> GetView() const { return view_; }

		YumeCamera* GetCullCamera() const;
		Ray GetScreenRay(int x,int y) const;
		IntVector2 WorldToScreenPoint(const Vector3& worldPos) const;
		Vector3 ScreenToWorldPoint(int x,int y,float depth) const;
		void AllocateView();

	private:
		WeakPtr<YumeCamera> camera_;
		WeakPtr<YumeCamera> cullCamera_;
		WeakPtr<YumeScene> scene_;
		IntRect rect_;
		SharedPtr<YumeRenderPipeline> renderPath_;
		SharedPtr<YumeRenderView> view_;
		bool drawDebug_;
	};
}


//----------------------------------------------------------------------------
#endif
