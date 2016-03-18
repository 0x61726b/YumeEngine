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
#include "YumeSkybox.h"
#include "YumeBatch.h"
#include "YumeGeometry.h"
#include "YumeDrawable.h"

#include "Scene/YumeSceneNode.h"

#include "YumeCamera.h"

namespace YumeEngine
{
	YumeHash YumeSkybox::type_ = "Skybox";
	YumeSkybox::YumeSkybox():
		lastFrame_(0)
	{
	}

	YumeSkybox::~YumeSkybox()
	{
	}


	void YumeSkybox::ProcessRayQuery(const RayOctreeQuery& query,YumeVector<RayQueryResult>::type& results)
	{
		// Do not record a raycast result for a skybox, as it would block all other results
	}

	void YumeSkybox::UpdateBatches(const FrameInfo& frame)
	{
		distance_ = 0.0f;

		if(frame.frameNumber_ != lastFrame_)
		{
			customWorldTransforms_.clear();
			lastFrame_ = frame.frameNumber_;
		}

		// Add camera position to fix the skybox in space. Use effective world transform to take reflection into account
		Matrix3x4 customWorldTransform = node_->GetWorldTransform();
		customWorldTransform.SetTranslation(node_->GetWorldPosition() + frame.camera_->GetEffectiveWorldTransform().Translation());
		std::pair<YumeMap<YumeCamera*,Matrix3x4>::iterator,bool> ret = customWorldTransforms_.insert(std::make_pair(frame.camera_,customWorldTransform));

		for(unsigned i = 0; i < batches_.size(); ++i)
		{
			batches_[i].worldTransform_ = &ret.first->second;
			batches_[i].distance_ = 0.0f;
		}
	}

	void YumeSkybox::OnWorldBoundingBoxUpdate()
	{
		// The skybox is supposed to be visible everywhere, so set a humongous bounding box
		worldBoundingBox_.Define(-M_LARGE_VALUE,M_LARGE_VALUE);
	}
}
