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
#include "YumeOctree.h"
#include "Renderer/YumeAuxRenderer.h"
#include "Renderer/YumeRHI.h"
#include "Scene/YumeScene.h"
#include "Core/YumeWorkQueue.h"

#include "Engine/YumeEngine.h"

#include "Logging/logging.h"

namespace YumeEngine
{

	static const float DEFAULT_OCTREE_SIZE = 1000.0f;
	static const int DEFAULT_OCTREE_LEVELS = 8;

	extern const char* SUBSYSTEM_CATEGORY;

	void UpdateDrawablesWork(const WorkItem* item,unsigned threadIndex)
	{
		const FrameInfo& frame = *(reinterpret_cast<FrameInfo*>(item->aux_));
		YumeDrawable** start = reinterpret_cast<YumeDrawable**>(item->start_);
		YumeDrawable** end = reinterpret_cast<YumeDrawable**>(item->end_);

		while(start != end)
		{
			YumeDrawable* drawable = *start;
			if(drawable)
				drawable->Update(frame);
			++start;
		}
	}

	inline bool CompareRayQueryResults(const RayQueryResult& lhs,const RayQueryResult& rhs)
	{
		return lhs.distance_ < rhs.distance_;
	}



	Octant::Octant(const BoundingBox& box,unsigned level,Octant* parent,Octree* root,unsigned index):
		level_(level),
		numDrawables_(0),
		parent_(parent),
		root_(root),
		index_(index)
	{
		Initialize(box);

		for(unsigned i = 0; i < NUM_OCTANTS; ++i)
			children_[i] = 0;
	}

	Octant::~Octant()
	{
		if(root_)
		{
			// Remove the drawables (if any) from this octant to the root octant
			for(YumeVector<YumeDrawable*>::iterator i = drawables_.begin(); i != drawables_.end(); ++i)
			{
				(*i)->SetOctant(root_);
				root_->drawables_.push_back(*i);
				root_->QueueUpdate(*i);
			}
			drawables_.clear();
			numDrawables_ = 0;
		}

		for(unsigned i = 0; i < NUM_OCTANTS; ++i)
			DeleteChild(i);
	}

	Octant* Octant::GetOrCreateChild(unsigned index)
	{
		if(children_[index])
			return children_[index];

		Vector3 newMin = worldBoundingBox_.min_;
		Vector3 newMax = worldBoundingBox_.max_;
		Vector3 oldCenter = worldBoundingBox_.Center();

		if(index & 1)
			newMin.x_ = oldCenter.x_;
		else
			newMax.x_ = oldCenter.x_;

		if(index & 2)
			newMin.y_ = oldCenter.y_;
		else
			newMax.y_ = oldCenter.y_;

		if(index & 4)
			newMin.z_ = oldCenter.z_;
		else
			newMax.z_ = oldCenter.z_;

		children_[index] = new Octant(BoundingBox(newMin,newMax),level_ + 1,this,root_,index);
		return children_[index];
	}

	void Octant::DeleteChild(unsigned index)
	{
		assert(index < NUM_OCTANTS);
		delete children_[index];
		children_[index] = 0;
	}

	void Octant::InsertDrawable(YumeDrawable* drawable)
	{
		const BoundingBox& box = drawable->GetWorldBoundingBox();

		// If root octant, insert all non-occludees here, so that octant occlusion does not hide the drawable.
		// Also if drawable is outside the root octant bounds, insert to root
		bool insertHere;
		if(this == root_)
			insertHere = !drawable->IsOccludee() || cullingBox_.IsInside(box) != INSIDE || CheckDrawableFit(box);
		else
			insertHere = CheckDrawableFit(box);

		if(insertHere)
		{
			Octant* oldOctant = drawable->octant_;
			if(oldOctant != this)
			{
				// Add first, then remove, because drawable count going to zero deletes the octree branch in question
				AddDrawable(drawable);
				if(oldOctant)
					oldOctant->RemoveDrawable(drawable,false);
			}
		}
		else
		{
			Vector3 boxCenter = box.Center();
			unsigned x = boxCenter.x_ < center_.x_ ? 0 : 1;
			unsigned y = boxCenter.y_ < center_.y_ ? 0 : 2;
			unsigned z = boxCenter.z_ < center_.z_ ? 0 : 4;

			GetOrCreateChild(x + y + z)->InsertDrawable(drawable);
		}
	}

	bool Octant::CheckDrawableFit(const BoundingBox& box) const
	{
		Vector3 boxSize = box.Size();

		// If max split level, size always OK, otherwise check that box is at least half size of octant
		if(level_ >= root_->GetNumLevels() || boxSize.x_ >= halfSize_.x_ || boxSize.y_ >= halfSize_.y_ ||
			boxSize.z_ >= halfSize_.z_)
			return true;
		// Also check if the box can not fit a child octant's culling box, in that case size OK (must insert here)
		else
		{
			if(box.min_.x_ <= worldBoundingBox_.min_.x_ - 0.5f * halfSize_.x_ ||
				box.max_.x_ >= worldBoundingBox_.max_.x_ + 0.5f * halfSize_.x_ ||
				box.min_.y_ <= worldBoundingBox_.min_.y_ - 0.5f * halfSize_.y_ ||
				box.max_.y_ >= worldBoundingBox_.max_.y_ + 0.5f * halfSize_.y_ ||
				box.min_.z_ <= worldBoundingBox_.min_.z_ - 0.5f * halfSize_.z_ ||
				box.max_.z_ >= worldBoundingBox_.max_.z_ + 0.5f * halfSize_.z_)
				return true;
		}

		// Bounding box too small, should create a child octant
		return false;
	}

	void Octant::ResetRoot()
	{
		root_ = 0;

		// The whole octree is being destroyed, just detach the drawables
		for(YumeVector<YumeDrawable*>::iterator i = drawables_.begin(); i != drawables_.end(); ++i)
			(*i)->SetOctant(0);

		for(unsigned i = 0; i < NUM_OCTANTS; ++i)
		{
			if(children_[i])
				children_[i]->ResetRoot();
		}
	}

	void Octant::DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest)
	{
		if(debug && debug->IsInside(worldBoundingBox_))
		{
			debug->AddBoundingBox(worldBoundingBox_,YumeColor(0.25f,0.25f,0.25f),depthTest);

			for(unsigned i = 0; i < NUM_OCTANTS; ++i)
			{
				if(children_[i])
					children_[i]->DrawDebugGeometry(debug,depthTest);
			}
		}
	}

	void Octant::Initialize(const BoundingBox& box)
	{
		worldBoundingBox_ = box;
		center_ = box.Center();
		halfSize_ = 0.5f * box.Size();
		cullingBox_ = BoundingBox(worldBoundingBox_.min_ - halfSize_,worldBoundingBox_.max_ + halfSize_);
	}

	void Octant::GetDrawablesInternal(OctreeQuery& query,bool inside) const
	{
		if(this != root_)
		{
			Intersection res = query.TestOctant(cullingBox_,inside);
			if(res == INSIDE)
				inside = true;
			else if(res == OUTSIDE)
			{
				// Fully outside, so cull this octant, its children & drawables
				return;
			}
		}

		if(drawables_.size())
		{
			YumeDrawable** start = const_cast<YumeDrawable**>(&drawables_[0]);
			YumeDrawable** end = start + drawables_.size();
			query.TestDrawables(start,end,inside);
		}

		for(unsigned i = 0; i < NUM_OCTANTS; ++i)
		{
			if(children_[i])
				children_[i]->GetDrawablesInternal(query,inside);
		}
	}

	void Octant::GetDrawablesInternal(RayOctreeQuery& query) const
	{
		float octantDist = query.ray_.HitDistance(cullingBox_);
		if(octantDist >= query.maxDistance_)
			return;

		if(drawables_.size())
		{
			YumeDrawable** start = const_cast<YumeDrawable**>(&drawables_[0]);
			YumeDrawable** end = start + drawables_.size();

			while(start != end)
			{
				YumeDrawable* drawable = *start++;

				if((drawable->GetDrawableFlags() & query.drawableFlags_) && (drawable->GetViewMask() & query.viewMask_))
					drawable->ProcessRayQuery(query,query.result_);
			}
		}

		for(unsigned i = 0; i < NUM_OCTANTS; ++i)
		{
			if(children_[i])
				children_[i]->GetDrawablesInternal(query);
		}
	}

	void Octant::GetDrawablesOnlyInternal(RayOctreeQuery& query,YumeVector<YumeDrawable*>::type& drawables) const
	{
		float octantDist = query.ray_.HitDistance(cullingBox_);
		if(octantDist >= query.maxDistance_)
			return;

		if(drawables_.size())
		{
			YumeDrawable** start = const_cast<YumeDrawable**>(&drawables_[0]);
			YumeDrawable** end = start + drawables_.size();

			while(start != end)
			{
				YumeDrawable* drawable = *start++;

				if((drawable->GetDrawableFlags() & query.drawableFlags_) && (drawable->GetViewMask() & query.viewMask_))
					drawables.push_back(drawable);
			}
		}

		for(unsigned i = 0; i < NUM_OCTANTS; ++i)
		{
			if(children_[i])
				children_[i]->GetDrawablesOnlyInternal(query,drawables);
		}
	}

	YumeHash Octree::type_ = "Octree";


	Octree::Octree():
		Octant(BoundingBox(-DEFAULT_OCTREE_SIZE,DEFAULT_OCTREE_SIZE),0,0,this),
		numLevels_(DEFAULT_OCTREE_LEVELS)
	{

	}

	Octree::~Octree()
	{
		// Reset root pointer from all child octants now so that they do not move their drawables to root
		drawableUpdates_.clear();
		drawableReinsertions_.clear();
		ResetRoot();
	}

	void Octree::DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest)
	{
		if(debug)
		{
			Octant::DrawDebugGeometry(debug,depthTest);
		}
	}

	void Octree::SetSize(const BoundingBox& box,unsigned numLevels)
	{

		// If drawables exist, they are temporarily moved to the root
		for(unsigned i = 0; i < NUM_OCTANTS; ++i)
			DeleteChild(i);

		Initialize(box);
		numDrawables_ = drawables_.size();
		numLevels_ = (unsigned)Max((int)numLevels,1);
	}

	void Octree::Update(const FrameInfo& frame)
	{
		// Let drawables update themselves before reinsertion. This can be used for animation
		if(!drawableUpdates_.empty())
		{
			// Perform updates in worker threads. Notify the scene that a threaded update is going on and components
			// (for example physics objects) should not perform non-threadsafe work when marked dirty
			YumeScene* scene = GetScene();
			YumeWorkQueue* queue = gYume->pWorkSystem;
			scene->BeginThreadedUpdate();

			int numWorkItems = queue->GetNumThreads() + 1; // Worker threads + main thread
			int drawablesPerItem = Max((int)(drawableUpdates_.size() / numWorkItems),1);

			YumeVector<YumeDrawable*>::iterator start = drawableUpdates_.begin();
			// Create a work item for each thread
			for(int i = 0; i < numWorkItems; ++i)
			{
				SharedPtr<WorkItem> item = queue->GetFreeItem();
				item->priority_ = M_MAX_UNSIGNED;
				item->workFunction_ = UpdateDrawablesWork;
				item->aux_ = const_cast<FrameInfo*>(&frame);

				YumeVector<YumeDrawable*>::iterator end = drawableUpdates_.end();
				int r = end - start;
				if(i < numWorkItems - 1 && r > drawablesPerItem)
					end = start + drawablesPerItem;

				item->start_ = &(*start);
				item->end_ = &(*end);
				queue->AddWorkItem(item);

				start = end;
			}

			queue->Complete(M_MAX_UNSIGNED);
			scene->EndThreadedUpdate();
		}

		// Notify drawable update being finished. Custom animation (eg. IK) can be done at this point
		YumeScene* scene = GetScene();
		if(scene)
		{

		}

		// Reinsert drawables that have been moved or resized, or that have been newly added to the octree and do not sit inside
		// the proper octant yet
		if(!drawableUpdates_.empty())
		{
			for(YumeVector<YumeDrawable*>::iterator i = drawableUpdates_.begin(); i != drawableUpdates_.end() ; ++i)
			{
				YumeDrawable* drawable = *i;
				drawable->updateQueued_ = false;
				Octant* octant = drawable->GetOctant();
				const BoundingBox& box = drawable->GetWorldBoundingBox();

				// Skip if no octant or does not belong to this octree anymore
				if(!octant || octant->GetRoot() != this)
					continue;
				// Skip if still fits the current octant
				if(drawable->IsOccludee() && octant->GetCullingBox().IsInside(box) == INSIDE && octant->CheckDrawableFit(box))
					continue;

				InsertDrawable(drawable);

#ifdef _DEBUG
				// Verify that the drawable will be culled correctly
				octant = drawable->GetOctant();
				if(octant != this && octant->GetCullingBox().IsInside(box) != INSIDE)
				{
					YUMELOG_ERROR("Drawable is not fully inside its octant's culling bounds: drawable box " + box.ToString() +
						" octant box " + octant->GetCullingBox().ToString());
				}
#endif
			}
		}

		drawableUpdates_.clear();
	}

	void Octree::AddManualDrawable(YumeDrawable* drawable)
	{
		if(!drawable || drawable->GetOctant())
			return;

		AddDrawable(drawable);
	}

	void Octree::RemoveManualDrawable(YumeDrawable* drawable)
	{
		if(!drawable)
			return;

		Octant* octant = drawable->GetOctant();
		if(octant && octant->GetRoot() == this)
			octant->RemoveDrawable(drawable);
	}

	void Octree::GetDrawables(OctreeQuery& query) const
	{
		query.result_.clear();
		GetDrawablesInternal(query,false);
	}

	void Octree::Raycast(RayOctreeQuery& query) const
	{
		query.result_.clear();
		GetDrawablesInternal(query);
		std::sort(query.result_.begin(),query.result_.end(),CompareRayQueryResults);
	}

	void Octree::RaycastSingle(RayOctreeQuery& query) const
	{
		query.result_.clear();
		rayQueryDrawables_.clear();
		GetDrawablesOnlyInternal(query,rayQueryDrawables_);

		// Sort by increasing hit distance to AABB
		for(YumeVector<YumeDrawable*>::iterator i = rayQueryDrawables_.begin(); i != rayQueryDrawables_.end(); ++i)
		{
			YumeDrawable* drawable = *i;
			drawable->SetSortValue(query.ray_.HitDistance(drawable->GetWorldBoundingBox()));
		}

		std::sort(rayQueryDrawables_.begin(),rayQueryDrawables_.end(),CompareDrawables);

		// Then do the actual test according to the query, and early-out as possible
		float closestHit = M_INFINITY;
		for(YumeVector<YumeDrawable*>::iterator i = rayQueryDrawables_.begin(); i != rayQueryDrawables_.end(); ++i)
		{
			YumeDrawable* drawable = *i;
			if(drawable->GetSortValue() < Min(closestHit,query.maxDistance_))
			{
				unsigned oldSize = query.result_.size();
				drawable->ProcessRayQuery(query,query.result_);
				if(query.result_.size() > oldSize)
					closestHit = Min(closestHit,query.result_.back().distance_);
			}
			else
				break;
		}

		if(query.result_.size() > 1)
		{
			std::sort(query.result_.begin(),query.result_.end(),CompareRayQueryResults);
			query.result_.resize(1);
		}
	}

	void Octree::QueueUpdate(YumeDrawable* drawable)
	{
		YumeScene* scene = GetScene();
		if(scene && scene->IsThreadedUpdate())
		{
			MutexLock lock(octreeMutex_);
			drawableUpdates_.push_back(drawable);
		}
		else
			drawableUpdates_.push_back(drawable);

		drawable->updateQueued_ = true;
	}

	void Octree::CancelUpdate(YumeDrawable* drawable)
	{
		drawableUpdates_.erase(std::find(drawableUpdates_.begin(),drawableUpdates_.end(),drawable));
		drawable->updateQueued_ = false;
	}

	void Octree::DrawDebugGeometry(bool depthTest)
	{
		YumeDebugRenderer* debug = GetComponent<YumeDebugRenderer>();
		DrawDebugGeometry(debug,depthTest);
	}

	void Octree::HandleRenderUpdate(float dt)
	{
		// When running in headless mode, update the Octree manually during the RenderUpdate event
		YumeScene* scene = GetScene();
		if(!scene || !scene->IsUpdateEnabled())
			return;

		FrameInfo frame;
		frame.frameNumber_ = gYume->pTimer->GetFrameNumber();
		frame.timeStep_ = dt;
		frame.camera_ = 0;

		Update(frame);
	}

}
