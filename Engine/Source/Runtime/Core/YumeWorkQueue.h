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
#ifndef __YumeWorkQuery_h__
#define __YumeWorkQuery_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Core/YumeVariant.h"

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class WorkerThread;

	/// Work queue item.
	struct WorkItem
	{
		friend class YumeWorkQueue;

	public:
		// Construct
		WorkItem():
			priority_(0),
			sendEvent_(false),
			completed_(false),
			pooled_(false)
		{
		}

		/// Work function. Called with the work item and thread index (0 = main thread) as parameters.
		void(*workFunction_)(const WorkItem*,unsigned);
		/// Data start pointer.
		void* start_;
		/// Data end pointer.
		void* end_;
		/// Auxiliary data pointer.
		void* aux_;
		/// Priority. Higher value = will be completed first.
		unsigned priority_;
		/// Whether to send event on completion.
		bool sendEvent_;
		/// Completed flag.
		volatile bool completed_;

	private:
		bool pooled_;
	};

	/// Work queue subsystem for multithreading.
	class YumeAPIExport YumeWorkQueue
	{
		friend class WorkerThread;

	public:
		/// Construct.
		YumeWorkQueue();
		/// Destruct.
		~YumeWorkQueue();

		/// Create worker threads. Can only be called once.
		void CreateThreads(unsigned numThreads);
		/// Get pointer to an usable WorkItem from the item pool. Allocate one if no more free items.
		SharedPtr<WorkItem> GetFreeItem();
		/// Add a work item and resume worker threads.
		void AddWorkItem(SharedPtr<WorkItem> item);
		/// Remove a work item before it has started executing. Return true if successfully removed.
		bool RemoveWorkItem(SharedPtr<WorkItem> item);
		/// Remove a number of work items before they have started executing. Return the number of items successfully removed.
		unsigned RemoveWorkItems(const YumeVector<SharedPtr<WorkItem> >::type& items);
		/// Pause worker threads.
		void Pause();
		/// Resume worker threads.
		void Resume();
		/// Finish all queued work which has at least the specified priority. Main thread will also execute priority work. Pause worker threads if no more work remains.
		void Complete(unsigned priority);

		/// Set the pool telerance before it starts deleting pool items.
		void SetTolerance(int tolerance) { tolerance_ = tolerance; }

		/// Set how many milliseconds maximum per frame to spend on low-priority work, when there are no worker threads.
		void SetNonThreadedWorkMs(int ms) { maxNonThreadedWorkMs_ = std::max(ms,1); }

		/// Return number of worker threads.
		unsigned GetNumThreads() const { return threads_.size(); }

		/// Return whether all work with at least the specified priority is finished.
		bool IsCompleted(unsigned priority) const;
		/// Return whether the queue is currently completing work in the main thread.
		bool IsCompleting() const { return completing_; }

		/// Return the pool tolerance.
		int GetTolerance() const { return tolerance_; }

		/// Return how many milliseconds maximum to spend on non-threaded low-priority work.
		int GetNonThreadedWorkMs() const { return maxNonThreadedWorkMs_; }

	private:
		/// Process work items until shut down. Called by the worker threads.
		void ProcessItems(unsigned threadIndex);
		/// Purge completed work items which have at least the specified priority, and send completion events as necessary.
		void PurgeCompleted(unsigned priority);
		/// Purge the pool to reduce allocation where its unneeded.
		void PurgePool();
		/// Return a work item to the pool.
		void ReturnToPool(SharedPtr<WorkItem>& item);
		/// Handle frame start event. Purge completed work from the main thread queue, and perform work if no threads at all.
		void HandleBeginFrame(YumeHash eventType,VariantMap& eventData);

		/// Worker threads.
		YumeVector<SharedPtr<WorkerThread> >::type threads_;
		/// Work item pool for reuse to cut down on allocation. The bool is a flag for item pooling and whether it is available or not.
		YumeVector<SharedPtr<WorkItem> >::type poolItems_;
		/// Work item collection. Accessed only by the main thread.
		YumeVector<SharedPtr<WorkItem> >::type workItems_;
		/// Work item prioritized queue for worker threads. Pointers are guaranteed to be valid (point to workItems.)
		YumeVector<WorkItem*>::type queue_;
		/// Worker queue mutex.
		boost::mutex queueMutex_;
		/// Shutting down flag.
		volatile bool shutDown_;
		/// Pausing flag. Indicates the worker threads should not contend for the queue mutex.
		volatile bool pausing_;
		/// Paused flag. Indicates the queue mutex being locked to prevent worker threads using up CPU time.
		bool paused_;
		/// Completing work in the main thread flag.
		bool completing_;
		/// Tolerance for the shared pool before it begins to deallocate.
		int tolerance_;
		/// Last size of the shared pool.
		unsigned lastSize_;
		/// Maximum milliseconds per frame to spend on low-priority work, when there are no worker threads.
		int maxNonThreadedWorkMs_;
	};

}


//----------------------------------------------------------------------------
#endif
