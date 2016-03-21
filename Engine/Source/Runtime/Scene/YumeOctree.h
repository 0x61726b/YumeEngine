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
#ifndef __YumeOctree_h__
#define __YumeOctree_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Renderer/YumeDrawable.h"
#include "YumeOctreeQuery.h"
#include "Core/YumeVariant.h"
#include "Core/YumeEventHub.h"

#include "Core/YumeMutex.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class Octree;

	static const int NUM_OCTANTS = 8;
	static const unsigned ROOT_INDEX = M_MAX_UNSIGNED;

	
	class YumeAPIExport Octant
	{
	public:
		
		Octant(const BoundingBox& box,unsigned level,Octant* parent,Octree* root,unsigned index = ROOT_INDEX);
		
		virtual ~Octant();

		
		Octant* GetOrCreateChild(unsigned index);
		
		void DeleteChild(unsigned index);
		
		void InsertDrawable(YumeDrawable * drawable);
		
		bool CheckDrawableFit(const BoundingBox& box) const;

		
		void AddDrawable(YumeDrawable * drawable)
		{
			drawable->SetOctant(this);
			drawables_.push_back(drawable);
			IncDrawableCount();
		}

		
		void RemoveDrawable(YumeDrawable* drawable,bool resetOctant = true)
		{
			YumeVector<YumeDrawable*>::iterator i = drawables_.find(drawable);
			if(i != drawables_.end())
			{
				drawables_.erase(i);
				if(resetOctant)
					drawable->SetOctant(0);
				DecDrawableCount();
			}
		}

		
		const BoundingBox& GetWorldBoundingBox() const { return worldBoundingBox_; }

		
		const BoundingBox& GetCullingBox() const { return cullingBox_; }

		
		unsigned GetLevel() const { return level_; }

		
		Octant* GetParent() const { return parent_; }

		
		Octree* GetRoot() const { return root_; }

		
		unsigned GetNumDrawables() const { return numDrawables_; }

		
		bool IsEmpty() { return numDrawables_ == 0; }

		
		void ResetRoot();
		
		void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

	protected:
		
		void Initialize(const BoundingBox& box);
		
		void GetDrawablesInternal(OctreeQuery& query,bool inside) const;
		
		void GetDrawablesInternal(RayOctreeQuery& query) const;
		
		void GetDrawablesOnlyInternal(RayOctreeQuery& query,YumeVector<YumeDrawable*>::type& drawables) const;

		
		void IncDrawableCount()
		{
			++numDrawables_;
			if(parent_)
				parent_->IncDrawableCount();
		}

		
		void DecDrawableCount()
		{
			Octant* parent = parent_;

			--numDrawables_;
			if(!numDrawables_)
			{
				if(parent)
					parent->DeleteChild(index_);
			}

			if(parent)
				parent->DecDrawableCount();
		}

		
		BoundingBox worldBoundingBox_;
		
		BoundingBox cullingBox_;
		
		YumeVector<YumeDrawable*>::type drawables_;
		
		Octant* children_[NUM_OCTANTS];
		
		Vector3 center_;
		
		Vector3 halfSize_;
		
		unsigned level_;
		
		unsigned numDrawables_;
		
		Octant* parent_;
		
		Octree* root_;
		
		unsigned index_;
	};

	
	class YumeAPIExport Octree : public YumeSceneComponent,public Octant,public EngineEventListener
	{
		friend void RaycastDrawablesWork(const WorkItem* item,unsigned threadIndex);
	public:
		Octree();
		~Octree();

		static YumeHash GetTypeStatic() { return type_; };
		virtual YumeHash GetType() { return type_; };
		static YumeHash type_;
		
		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		
		void SetSize(const BoundingBox& box,unsigned numLevels);
		
		void Update(const FrameInfo& frame);
		
		void AddManualDrawable(YumeDrawable* drawable);
		
		void RemoveManualDrawable(YumeDrawable* drawable);

		
		void GetDrawables(OctreeQuery& query) const;
		
		void Raycast(RayOctreeQuery& query) const;
		
		void RaycastSingle(RayOctreeQuery& query) const;

		
		unsigned GetNumLevels() const { return numLevels_; }

		
		void QueueUpdate(YumeDrawable* drawable);
		
		void CancelUpdate(YumeDrawable* drawable);
		
		void DrawDebugGeometry(bool depthTest);

	private:
		
		void HandleRenderUpdate(float dt);

		
		YumeVector<YumeDrawable*>::type drawableUpdates_;
		
		YumeVector<YumeDrawable*>::type drawableReinsertions_;
		
		Mutex octreeMutex_;
		
		mutable YumeVector<YumeDrawable*>::type rayQueryDrawables_;
		
		unsigned numLevels_;
	};
}


//----------------------------------------------------------------------------
#endif
