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
#include "YumeDrawable.h"
#include "YumeCamera.h"
#include "YumeRendererEnv.h"
#include "YumeRenderer.h"
#include "YumeVertexBuffer.h"
#include "YumeMaterial.h"
#include "Scene/YumeOctree.h"
#include "Scene/YumeScene.h"
#include "YumeAuxRenderer.h"
#include "YumeGeometry.h"
#include "Engine/YumeEngine.h"


#include "Logging/logging.h"

namespace YumeEngine
{

	const char* GEOMETRY_CATEGORY = "Geometry";

	SourceBatch::SourceBatch():
		distance_(0.0f),
		geometry_(0),
		worldTransform_(&Matrix3x4::IDENTITY),
		numWorldTransforms_(1),
		geometryType_(GEOM_STATIC)
	{
	}

	SourceBatch::SourceBatch(const SourceBatch& batch)
	{
		*this = batch;
	}

	SourceBatch::~SourceBatch()
	{
	}

	SourceBatch& SourceBatch::operator =(const SourceBatch& rhs)
	{
		distance_ = rhs.distance_;
		geometry_ = rhs.geometry_;
		material_ = rhs.material_;
		worldTransform_ = rhs.worldTransform_;
		numWorldTransforms_ = rhs.numWorldTransforms_;
		geometryType_ = rhs.geometryType_;

		return *this;
	}


	YumeDrawable::YumeDrawable(unsigned char drawableFlags):
		boundingBox_(0.0f,0.0f),
		drawableFlags_(drawableFlags),
		worldBoundingBoxDirty_(true),
		castShadows_(false),
		occluder_(false),
		occludee_(true),
		updateQueued_(false),
		zoneDirty_(false),
		octant_(0),
		zone_(0),
		viewMask_(DEFAULT_VIEWMASK),
		lightMask_(DEFAULT_LIGHTMASK),
		shadowMask_(DEFAULT_SHADOWMASK),
		zoneMask_(DEFAULT_ZONEMASK),
		viewFrameNumber_(0),
		distance_(0.0f),
		lodDistance_(0.0f),
		drawDistance_(0.0f),
		shadowDistance_(0.0f),
		sortValue_(0.0f),
		minZ_(0.0f),
		maxZ_(0.0f),
		lodBias_(1.0f),
		basePassFlags_(0),
		maxLights_(0),
		firstLight_(0)
	{
	}

	YumeDrawable::~YumeDrawable()
	{
		RemoveFromOctree();
	}

	void YumeDrawable::OnSetEnabled()
	{
		bool enabled = IsEnabledEffective();

		if(enabled && !octant_)
			AddToOctree();
		else if(!enabled && octant_)
			RemoveFromOctree();
	}

	void YumeDrawable::ProcessRayQuery(const RayOctreeQuery& query,YumeVector<RayQueryResult>::type& results)
	{
		float distance = query.ray_.HitDistance(GetWorldBoundingBox());
		if(distance < query.maxDistance_)
		{
			RayQueryResult result;
			result.position_ = query.ray_.origin_ + distance * query.ray_.direction_;
			result.normal_ = -query.ray_.direction_;
			result.distance_ = distance;
			result.drawable_ = this;
			result.node_ = GetNode();
			result.subObject_ = M_MAX_UNSIGNED;
			results.push_back(result);
		}
	}

	void YumeDrawable::Update(const FrameInfo& frame)
	{
	}

	void YumeDrawable::UpdateBatches(const FrameInfo& frame)
	{
		const BoundingBox& worldBoundingBox = GetWorldBoundingBox();
		const Matrix3x4& worldTransform = node_->GetWorldTransform();
		distance_ = frame.camera_->GetDistance(worldBoundingBox.Center());

		for(unsigned i = 0; i < batches_.size(); ++i)
		{
			batches_[i].distance_ = distance_;
			batches_[i].worldTransform_ = &worldTransform;
		}

		float scale = worldBoundingBox.Size().DotProduct(DOT_SCALE);
		float newLodDistance = frame.camera_->GetLodDistance(distance_,scale,lodBias_);

		if(newLodDistance != lodDistance_)
			lodDistance_ = newLodDistance;
	}

	void YumeDrawable::UpdateGeometry(const FrameInfo& frame)
	{
	}

	YumeGeometry* YumeDrawable::GetLodGeometry(unsigned batchIndex,unsigned level)
	{
		// By default return the visible batch geometry
		if(batchIndex < batches_.size())
			return batches_[batchIndex].geometry_;
		else
			return 0;
	}

	bool YumeDrawable::DrawOcclusion(OcclusionBuffer* buffer)
	{
		return true;
	}

	void YumeDrawable::DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest)
	{
		if(debug && IsEnabledEffective())
			debug->AddBoundingBox(GetWorldBoundingBox(),YumeColor::GREEN,depthTest);
	}

	void YumeDrawable::SetDrawDistance(float distance)
	{
		drawDistance_ = distance;
		
	}

	void YumeDrawable::SetShadowDistance(float distance)
	{
		shadowDistance_ = distance;
		
	}

	void YumeDrawable::SetLodBias(float bias)
	{
		lodBias_ = Max(bias,M_EPSILON);
		
	}

	void YumeDrawable::SetViewMask(unsigned mask)
	{
		viewMask_ = mask;
		
	}

	void YumeDrawable::SetLightMask(unsigned mask)
	{
		lightMask_ = mask;
		
	}

	void YumeDrawable::SetShadowMask(unsigned mask)
	{
		shadowMask_ = mask;
		
	}

	void YumeDrawable::SetZoneMask(unsigned mask)
	{
		zoneMask_ = mask;
		// Mark dirty to reset cached zone
		OnMarkedDirty(node_);
		
	}

	void YumeDrawable::SetMaxLights(unsigned num)
	{
		maxLights_ = num;
		
	}

	void YumeDrawable::SetCastShadows(bool enable)
	{
		castShadows_ = enable;
		
	}

	void YumeDrawable::SetOccluder(bool enable)
	{
		occluder_ = enable;
		
	}

	void YumeDrawable::SetOccludee(bool enable)
	{
		if(enable != occludee_)
		{
			occludee_ = enable;
			// Reinsert to octree to make sure octant occlusion does not erroneously hide this drawable
			if(octant_ && !updateQueued_)
				octant_->GetRoot()->QueueUpdate(this);
			
		}
	}

	void YumeDrawable::MarkForUpdate()
	{
		if(!updateQueued_ && octant_)
			octant_->GetRoot()->QueueUpdate(this);
	}

	const BoundingBox& YumeDrawable::GetWorldBoundingBox()
	{
		if(worldBoundingBoxDirty_)
		{
			OnWorldBoundingBoxUpdate();
			worldBoundingBoxDirty_ = false;
		}

		return worldBoundingBox_;
	}

	bool YumeDrawable::IsInView() const
	{
		// Note: in headless mode there is no renderer subsystem and no view frustum tests are performed, so return
		// always false in that case
		YumeRenderer* renderer = YumeEngine3D::Get()->GetRenderLogic().get();
		return false;
	}

	bool YumeDrawable::IsInView(YumeCamera* camera) const
	{
		YumeRenderer* renderer = YumeEngine3D::Get()->GetRenderLogic().get();
		/*return renderer && viewFrameNumber_ == renderer->GetFrameInfo().frameNumber_ && (!camera || std::find(viewCameras_.begin(),viewCameras_.end(),camera) == viewCameras_.end());*/
		return false;
	}

	bool YumeDrawable::IsInView(const FrameInfo& frame,bool anyCamera) const
	{
		return viewFrameNumber_ == frame.frameNumber_ && (anyCamera || std::find(viewCameras_.begin(),viewCameras_.end(),frame.camera_) == viewCameras_.end());
	}

	void YumeDrawable::SetZone(YumeRendererEnvironment* zone,bool temporary)
	{
		zone_ = zone;

		// If the zone assignment was temporary (inconclusive) set the dirty flag so that it will be re-evaluated on the next frame
		zoneDirty_ = temporary;
	}

	void YumeDrawable::SetSortValue(float value)
	{
		sortValue_ = value;
	}

	void YumeDrawable::MarkInView(const FrameInfo& frame)
	{
		if(frame.frameNumber_ != viewFrameNumber_)
		{
			viewFrameNumber_ = frame.frameNumber_;
			viewCameras_.resize(1);
			viewCameras_[0] = frame.camera_;
		}
		else
			viewCameras_.push_back(frame.camera_);

		basePassFlags_ = 0;
		firstLight_ = 0;
		lights_.clear();
		vertexLights_.clear();
	}

	void YumeDrawable::MarkInView(unsigned frameNumber)
	{
		if(frameNumber != viewFrameNumber_)
		{
			viewFrameNumber_ = frameNumber;
			viewCameras_.clear();
		}
	}

	void YumeDrawable::LimitLights()
	{
		// Maximum lights value 0 means unlimited
		if(!maxLights_ || lights_.size() <= maxLights_)
			return;

		// If more lights than allowed, move to vertex lights and cut the list
		const BoundingBox& box = GetWorldBoundingBox();
		for(unsigned i = 0; i < lights_.size(); ++i)
			lights_[i]->SetIntensitySortValue(box);

		std::sort(lights_.begin(),lights_.end(),CompareDrawables);
		vertexLights_.insert(vertexLights_.end(),lights_.begin() + maxLights_,lights_.end());
		lights_.resize(maxLights_);
	}

	void YumeDrawable::LimitVertexLights(bool removeConvertedLights)
	{
		if(removeConvertedLights)
		{
			for(unsigned i = vertexLights_.size() - 1; i < vertexLights_.size(); --i)
			{
				if(!vertexLights_[i]->GetPerVertex())
					vertexLights_.erase(vertexLights_.begin()+i);
			}
		}

		if(vertexLights_.size() <= MAX_VERTEX_LIGHTS)
			return;

		const BoundingBox& box = GetWorldBoundingBox();
		for(unsigned i = 0; i < vertexLights_.size(); ++i)
			vertexLights_[i]->SetIntensitySortValue(box);

		std::sort(vertexLights_.begin(),vertexLights_.end(),CompareDrawables);
		vertexLights_.resize(MAX_VERTEX_LIGHTS);
	}

	void YumeDrawable::OnNodeSet(YumeSceneNode* node)
	{
		if(node)
			node->AddListener(this);
	}

	void YumeDrawable::OnSceneSet(YumeScene* scene)
	{
		if(scene)
			AddToOctree();
		else
			RemoveFromOctree();
	}

	void YumeDrawable::OnMarkedDirty(YumeSceneNode* node)
	{
		worldBoundingBoxDirty_ = true;
		if(!updateQueued_ && octant_)
			octant_->GetRoot()->QueueUpdate(this);

		// Mark zone assignment dirty when transform changes
		if(node == node_)
			zoneDirty_ = true;
	}

	void YumeDrawable::AddToOctree()
	{
		// Do not add to octree when disabled
		if(!IsEnabledEffective())
			return;

		YumeScene* scene = GetScene();
		if(scene)
		{
			YumeHash t = Octree::GetType();
			Octree* octree = static_cast<Octree*>(scene->GetComponent(t));
			if(octree)
				octree->InsertDrawable(this);
			else
				YUMELOG_ERROR("No Octree component in scene, drawable will not render");
		}
		else
		{
			// We have a mechanism for adding detached nodes to an octree manually, so do not log this error
			//URHO3D_LOGERROR("Node is detached from scene, drawable will not render");
		}
	}

	void YumeDrawable::RemoveFromOctree()
	{
		if(octant_)
		{
			Octree* octree = octant_->GetRoot();
			if(updateQueued_)
				octree->CancelUpdate(this);

			// Perform subclass specific deinitialization if necessary
			OnRemoveFromOctree();

			octant_->RemoveDrawable(this);
		}
	}
}
