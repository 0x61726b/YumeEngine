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

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{

	class Octree;

	static const int NUM_OCTANTS = 8;
	static const unsigned ROOT_INDEX = M_MAX_UNSIGNED;

	/// %Octree octant
	class YumeAPIExport Octant
	{
	public:
		/// Construct.
		Octant(const BoundingBox& box,unsigned level,Octant* parent,Octree* root,unsigned index = ROOT_INDEX);
		/// Destruct. Move drawables to root if available (detach if not) and free child octants.
		virtual ~Octant();

		/// Return or create a child octant.
		Octant* GetOrCreateChild(unsigned index);
		/// Delete child octant.
		void DeleteChild(unsigned index);
		/// Insert a drawable object by checking for fit recursively.
		void InsertDrawable(YumeDrawable * drawable);
		/// Check if a drawable object fits.
		bool CheckDrawableFit(const BoundingBox& box) const;

		/// Add a drawable object to this octant.
		void AddDrawable(YumeDrawable * drawable)
		{
			drawable->SetOctant(this);
			drawables_.push_back(drawable);
			IncDrawableCount();
		}

		/// Remove a drawable object from this octant.
		void RemoveDrawable(YumeDrawable* drawable,bool resetOctant = true)
		{
			YumeVector<YumeDrawable*>::iterator i = std::find(drawables_.begin(),drawables_.end(),drawable);
			if(i != drawables_.end())
			{
				if(resetOctant)
					drawable->SetOctant(0);
				DecDrawableCount();
			}
		}

		/// Return world-space bounding box.
		const BoundingBox& GetWorldBoundingBox() const { return worldBoundingBox_; }

		/// Return bounding box used for fitting drawable objects.
		const BoundingBox& GetCullingBox() const { return cullingBox_; }

		/// Return subdivision level.
		unsigned GetLevel() const { return level_; }

		/// Return parent octant.
		Octant* GetParent() const { return parent_; }

		/// Return octree root.
		Octree* GetRoot() const { return root_; }

		/// Return number of drawables.
		unsigned GetNumDrawables() const { return numDrawables_; }

		/// Return true if there are no drawable objects in this octant and child octants.
		bool IsEmpty() { return numDrawables_ == 0; }

		/// Reset root pointer recursively. Called when the whole octree is being destroyed.
		void ResetRoot();
		/// Draw bounds to the debug graphics recursively.
		void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

	protected:
		/// Initialize bounding box.
		void Initialize(const BoundingBox& box);
		/// Return drawable objects by a query, called internally.
		void GetDrawablesInternal(OctreeQuery& query,bool inside) const;
		/// Return drawable objects by a ray query, called internally.
		void GetDrawablesInternal(RayOctreeQuery& query) const;
		/// Return drawable objects only for a threaded ray query, called internally.
		void GetDrawablesOnlyInternal(RayOctreeQuery& query,YumeVector<YumeDrawable*>::type& drawables) const;

		/// Increase drawable object count recursively.
		void IncDrawableCount()
		{
			++numDrawables_;
			if(parent_)
				parent_->IncDrawableCount();
		}

		/// Decrease drawable object count recursively and remove octant if it becomes empty.
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

		/// World bounding box.
		BoundingBox worldBoundingBox_;
		/// Bounding box used for drawable object fitting.
		BoundingBox cullingBox_;
		/// Drawable objects.
		YumeVector<YumeDrawable*>::type drawables_;
		/// Child octants.
		Octant* children_[NUM_OCTANTS];
		/// World bounding box center.
		Vector3 center_;
		/// World bounding box half size.
		Vector3 halfSize_;
		/// Subdivision level.
		unsigned level_;
		/// Number of drawable objects in this octant and child octants.
		unsigned numDrawables_;
		/// Parent octant.
		Octant* parent_;
		/// Octree root.
		Octree* root_;
		/// Octant index relative to its siblings or ROOT_INDEX for root octant
		unsigned index_;
	};

	/// %Octree component. Should be added only to the root scene node
	class YumeAPIExport Octree : public YumeSceneComponent,public Octant,public EngineEventListener
	{
		friend void RaycastDrawablesWork(const WorkItem* item,unsigned threadIndex);
	public:
		/// Construct.
		Octree();
		/// Destruct.
		~Octree();

		static YumeHash type_;
		static YumeHash GetType();

		/// Visualize the component as debug geometry.
		virtual void DrawDebugGeometry(YumeDebugRenderer* debug,bool depthTest);

		/// Set size and maximum subdivision levels. If octree is not empty, drawable objects will be temporarily moved to the root.
		void SetSize(const BoundingBox& box,unsigned numLevels);
		/// Update and reinsert drawable objects.
		void Update(const FrameInfo& frame);
		/// Add a drawable manually.
		void AddManualDrawable(YumeDrawable* drawable);
		/// Remove a manually added drawable.
		void RemoveManualDrawable(YumeDrawable* drawable);

		/// Return drawable objects by a query.
		void GetDrawables(OctreeQuery& query) const;
		/// Return drawable objects by a ray query.
		void Raycast(RayOctreeQuery& query) const;
		/// Return the closest drawable object by a ray query.
		void RaycastSingle(RayOctreeQuery& query) const;

		/// Return subdivision levels.
		unsigned GetNumLevels() const { return numLevels_; }

		/// Mark drawable object as requiring an update and a reinsertion.
		void QueueUpdate(YumeDrawable* drawable);
		/// Cancel drawable object's update.
		void CancelUpdate(YumeDrawable* drawable);
		/// Visualize the component as debug geometry.
		void DrawDebugGeometry(bool depthTest);

	private:
		/// Handle render update in case of headless execution.
		void HandleRenderUpdate(float dt);

		/// Drawable objects that require update.
		YumeVector<YumeDrawable*>::type drawableUpdates_;
		/// Drawable objects that require reinsertion.
		YumeVector<YumeDrawable*>::type drawableReinsertions_;
		/// Mutex for octree reinsertions.
		boost::mutex octreeMutex_;
		/// Ray query temporary list of drawables.
		mutable YumeVector<YumeDrawable*>::type rayQueryDrawables_;
		/// Subdivision level.
		unsigned numLevels_;
	};
}


//----------------------------------------------------------------------------
#endif
