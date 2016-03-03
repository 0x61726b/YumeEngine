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
	class YumeRenderPath;
	class YumeRenderPath;
	class YumeView;


	/// %Viewport definition either for a render surface or the backbuffer.
	class YumeAPIExport YumeViewport
	{
	public:
		/// Construct with defaults.
		YumeViewport();
		/// Construct with a full rectangle.
		YumeViewport(YumeCamera* camera,YumeRenderPath* renderPath = 0);
		/// Construct with a specified rectangle.
		YumeViewport(YumeCamera* camera,const IntRect& rect,YumeRenderPath* renderPath = 0);
		/// Destruct.
		~YumeViewport();

	//	void SetCamera(YumeCamera* camera);
	//	/// Set rectangle.
	//	void SetRect(const IntRect& rect);
	//	/// Set rendering path.
		void SetRenderPath(YumeRenderPath* path);
	//	/// Set rendering path from an XML file.
	//	void SetRenderPath();
	//	/// Set whether to render debug geometry. Default true.
	//	void SetDrawDebug(bool enable);
	//	/// Set separate camera to use for culling. Sharing a culling camera between several viewports allows to prepare the view only once, saving in CPU use. The culling camera's frustum should cover all the viewport cameras' frusta or else objects may be missing from the rendered view.
	//	void SetCullCamera(YumeCamera* camera);

	//	/// Return viewport camera.
	//	YumeCamera* GetCamera() const;
	//	/// Return the internal rendering structure. May be null if the viewport has not been rendered yet.
	//	YumeView* GetView() const;

	//	/// Return rectangle.
	//	const IntRect& GetRect() const { return rect_; }

	//	/// Return rendering path.
	//	YumeRenderPath* GetRenderPath() const;

	//	/// Return whether to draw debug geometry.
	//	bool GetDrawDebug() const { return drawDebug_; }

	//	/// Return the culling camera. If null, the viewport camera will be used for culling (normal case.)
	//	YumeCamera* GetCullCamera() const;

	//	/// Return ray corresponding to normalized screen coordinates.
	//	Ray GetScreenRay(int x,int y) const;
	//	// Convert a world space point to normalized screen coordinates.
	//	Vector2 WorldToScreenPoint(const Vector3& worldPos) const;
	//	// Convert screen coordinates and depth to a world space point.
	//	Vector3 ScreenToWorldPoint(int x,int y,float depth) const;

	//	/// Allocate the view structure. Called by Renderer.
	//	void AllocateView();

	private:
		/// Camera pointer.
		YumeCamera* camera_;
		/// Culling camera pointer.
		YumeCamera* cullCamera_;
		/// Viewport rectangle.
		IntRect rect_;
		/// Rendering path.
		SharedPtr<YumeRenderPath> renderPath_;
		/// Internal rendering structure.
		SharedPtr<YumeView> view_;
		/// Debug draw flag.
		bool drawDebug_;
	};
}


//----------------------------------------------------------------------------
#endif
