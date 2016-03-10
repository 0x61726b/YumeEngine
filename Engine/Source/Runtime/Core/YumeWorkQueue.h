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

		
		void(*workFunction_)(const WorkItem*,unsigned);
		
		void* start_;
		
		void* end_;
		
		void* aux_;
		
		unsigned priority_;
		
		bool sendEvent_;
		
		volatile bool completed_;

	private:
		bool pooled_;
	};

	
	class YumeAPIExport YumeWorkQueue
	{
		friend class WorkerThread;

	public:
		
		YumeWorkQueue();
		
		~YumeWorkQueue();

		
		void CreateThreads(unsigned numThreads);
		
		SharedPtr<WorkItem> GetFreeItem();
		
		void AddWorkItem(SharedPtr<WorkItem> item);
		
		bool RemoveWorkItem(SharedPtr<WorkItem> item);
		
		unsigned RemoveWorkItems(const YumeVector<SharedPtr<WorkItem> >::type& items);
		
		void Pause();
		
		void Resume();
		
		void Complete(unsigned priority);

		
		void SetTolerance(int tolerance) { tolerance_ = tolerance; }

		
		void SetNonThreadedWorkMs(int ms) { maxNonThreadedWorkMs_ = std::max(ms,1); }

		
		unsigned GetNumThreads() const { return threads_.size(); }

		
		bool IsCompleted(unsigned priority) const;
		
		bool IsCompleting() const { return completing_; }

		
		int GetTolerance() const { return tolerance_; }

		
		int GetNonThreadedWorkMs() const { return maxNonThreadedWorkMs_; }

	private:
		
		void ProcessItems(unsigned threadIndex);
		
		void PurgeCompleted(unsigned priority);
		
		void PurgePool();
		
		void ReturnToPool(SharedPtr<WorkItem>& item);
		
		void HandleBeginFrame(YumeHash eventType,VariantMap& eventData);

		
		YumeVector<SharedPtr<WorkerThread> >::type threads_;
		
		YumeVector<SharedPtr<WorkItem> >::type poolItems_;
		
		YumeVector<SharedPtr<WorkItem> >::type workItems_;
		
		YumeVector<WorkItem*>::type queue_;
		
		boost::mutex queueMutex_;
		
		volatile bool shutDown_;
		
		volatile bool pausing_;
		
		bool paused_;
		
		bool completing_;
		
		int tolerance_;
		
		unsigned lastSize_;
		
		int maxNonThreadedWorkMs_;
	};

}


//----------------------------------------------------------------------------
#endif
