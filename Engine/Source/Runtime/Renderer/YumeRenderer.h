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
#ifndef __YumeRenderer_h__
#define __YumeRenderer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeDrawable.h"
#include "YumeViewport.h"
#include <boost/weak_ptr.hpp>

#include "Core/YumeEventHub.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeRHI;
	class YumeRenderable;
	class YumeVertexBuffer;
	class YumeIndexBuffer;
	class YumeGeometry;
	class YumeRenderPipeline;

	class YumeAPIExport YumeRenderer : public EngineEventListener
	{
	public:
		YumeRenderer(YumeRHI*);
		virtual ~YumeRenderer();

		void Render();
		void Update(float timeStep);
		void QueueRenderable(YumeRenderable* renderTarget);

		void SetViewport(int index,SharedPtr<YumeViewport> viewport);

		void Initialize();

		void HandleRenderUpdate(float timeStep);
		//Getters


		int GetTextureQuality() const { return textureQuality_; }
		YumeRenderPipeline* GetDefaultPipeline() { return defaultPipeline_; }

	protected:
		int textureQuality_;

	private:
		YumeRHI* rhi_;
		FrameInfo frame_;

		YumeVector<SharedPtr<YumeViewport> >::type viewports_;

		YumeIndexBuffer* ib_;
		std::vector<unsigned> elementMasks_;
		std::vector<YumeVertexBuffer*> vbs_;
		SharedPtr<YumeGeometry> debugGeo_;

		YumeRenderPipeline* defaultPipeline_;
	};
}


//----------------------------------------------------------------------------
#endif
