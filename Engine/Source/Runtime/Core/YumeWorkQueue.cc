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
#include "YumeWorkQueue.h"
#include "Core/YumeThread.h"
#include "Logging/logging.h"
#include "Core/YumeTimer.h"
#include "Math/YumeMath.h"

namespace YumeEngine
{

	
	class WorkerThread : public YumeThreadWrapper
	{
	public:
		
		WorkerThread(YumeWorkQueue* owner,unsigned index):
			owner_(owner),
			index_(index)
		{
		}

		
		virtual void ThreadRunner()
		{
			owner_->ProcessItems(index_);
		}

		
		unsigned GetIndex() const { return index_; }

	private:
		
		YumeWorkQueue* owner_;
		
		unsigned index_;
	};

	YumeWorkQueue::YumeWorkQueue():
		shutDown_(false),
		pausing_(false),
		paused_(false),
		completing_(false),
		tolerance_(10),
		lastSize_(0),
		maxNonThreadedWorkMs_(5)
	{

	}

	YumeWorkQueue::~YumeWorkQueue()
	{
		// Stop the worker threads. First make sure they are not waiting for work items
		shutDown_ = true;
		Resume();

		for(unsigned i = 0; i < threads_.size(); ++i)
			threads_[i]->Stop();
	}

	void YumeWorkQueue::CreateThreads(unsigned numThreads)
	{

		if(!threads_.empty())
			return;

		// Start threads in paused mode
		Pause();

		

		for(unsigned i = 0; i < numThreads; ++i)
		{
			YUMELOG_INFO("Creating worker thread #" << i);
			SharedPtr<WorkerThread> thread(new WorkerThread(this,i + 1));
			thread->Run();
			threads_.push_back(thread);
		}
	}

	SharedPtr<WorkItem> YumeWorkQueue::GetFreeItem()
	{
		if(poolItems_.size() > 0)
		{
			SharedPtr<WorkItem> item = poolItems_.front();
			poolItems_.erase(poolItems_.begin());
			return item;
		}
		else
		{
			// No usable items found, create a new one set it as pooled and return it.
			SharedPtr<WorkItem> item(new WorkItem());
			item->pooled_ = true;
			return item;
		}
	}

	void YumeWorkQueue::AddWorkItem(SharedPtr<WorkItem> item)
	{
		if(!item)
		{
			YUMELOG_ERROR("Null work item submitted to the work queue");
			return;
		}
		assert(std::find(workItems_.begin(),workItems_.end(),item) != workItems_.end());
			

		// Push to the main thread list to keep item alive
		// Clear completed flag in case item is reused
		workItems_.push_back(item);
		item->completed_ = false;

		// Make sure worker threads' list is safe to modify
		if(threads_.size() && !paused_)
			queueMutex_.lock();

		// Find position for new item
		if(queue_.empty())
			queue_.push_back(item.get());
		else
		{
			for(YumeVector<WorkItem*>::iterator i = queue_.begin(); i != queue_.end(); ++i)
			{
				if((*i)->priority_ <= item->priority_)
				{
					queue_.insert(i,item.get());
					break;
				}
			}
		}

		if(threads_.size())
		{
			queueMutex_.unlock();
			paused_ = false;
		}
	}

	bool YumeWorkQueue::RemoveWorkItem(SharedPtr<WorkItem> item)
	{
		if(!item)
			return false;

		boost::mutex::scoped_lock lock(queueMutex_);

		// Can only remove successfully if the item was not yet taken by threads for execution
		YumeVector<WorkItem*>::iterator i = std::find(queue_.begin(),queue_.end(),item.get());
		if(i != queue_.end())
		{
			YumeVector<SharedPtr<WorkItem> >::iterator j = std::find(workItems_.begin(),workItems_.end(),item);
			if(j != workItems_.end())
			{
				queue_.erase(i);
				ReturnToPool(item);
				workItems_.erase(j);
				return true;
			}
		}

		return false;
	}

	unsigned YumeWorkQueue::RemoveWorkItems(const YumeVector<SharedPtr<WorkItem> >::type& items)
	{
		boost::mutex::scoped_lock lock(queueMutex_);
		unsigned removed = 0;

		for(YumeVector<SharedPtr<WorkItem> >::const_iterator i = items.begin(); i != items.end(); ++i)
		{
			YumeVector<WorkItem*>::iterator j = std::find(queue_.begin(),queue_.end(),i->get());
			if(j != queue_.end())
			{
				YumeVector<SharedPtr<WorkItem> >::iterator k = std::find(workItems_.begin(),workItems_.end(),*i);
				if(k != workItems_.end())
				{
					queue_.erase(j);
					ReturnToPool(*k);
					workItems_.erase(k);
					++removed;
				}
			}
		}

		return removed;
	}

	void YumeWorkQueue::Pause()
	{
		if(!paused_)
		{
			pausing_ = true;

			queueMutex_.lock();
			paused_ = true;

			pausing_ = false;
		}
	}

	void YumeWorkQueue::Resume()
	{
		if(paused_)
		{
			queueMutex_.unlock();
			paused_ = false;
		}
	}


	void YumeWorkQueue::Complete(unsigned priority)
	{
		completing_ = true;

		if(threads_.size())
		{
			Resume();

			// Take work items also in the main thread until queue empty or no high-priority items anymore
			while(!queue_.empty())
			{
				queueMutex_.lock();
				if(!queue_.empty() && queue_.front()->priority_ >= priority)
				{
					WorkItem* item = queue_.front();
					queue_.erase(queue_.begin());
					queueMutex_.unlock();
					item->workFunction_(item,0);
					item->completed_ = true;
				}
				else
				{
					queueMutex_.unlock();
					break;
				}
			}

			// Wait for threaded work to complete
			while(!IsCompleted(priority))
			{
			}

			// If no work at all remaining, pause worker threads by leaving the mutex locked
			if(queue_.empty())
				Pause();
		}
		else
		{
			// No worker threads: ensure all high-priority items are completed in the main thread
			while(!queue_.empty() && queue_.front()->priority_ >= priority)
			{
				WorkItem* item = queue_.front();
				queue_.erase(queue_.begin());
				item->workFunction_(item,0);
				item->completed_ = true;
			}
		}

		PurgeCompleted(priority);
		completing_ = false;
	}

	bool YumeWorkQueue::IsCompleted(unsigned priority) const
	{
		for(YumeVector<SharedPtr<WorkItem> >::const_iterator i = workItems_.begin(); i != workItems_.end(); ++i)
		{
			if((*i)->priority_ >= priority && !(*i)->completed_)
				return false;
		}

		return true;
	}

	void YumeWorkQueue::ProcessItems(unsigned threadIndex)
	{
		bool wasActive = false;

		for(;;)
		{
			if(shutDown_)
				return;

			if(pausing_ && !wasActive)
				YumeTime::Sleep(0);
			else
			{
				queueMutex_.lock();
				if(!queue_.empty())
				{
					wasActive = true;

					WorkItem* item = queue_.front();
					queue_.erase(queue_.begin());
					queueMutex_.unlock();
					item->workFunction_(item,threadIndex);
					item->completed_ = true;
				}
				else
				{
					wasActive = false;

					queueMutex_.unlock();
					YumeTime::Sleep(0);
				}
			}
		}
	}

	void YumeWorkQueue::PurgeCompleted(unsigned priority)
	{
		// Purge completed work items and send completion events. Do not signal items lower than priority threshold,
		// as those may be user submitted and lead to eg. scene manipulation that could happen in the middle of the
		// render update, which is not allowed
		for(YumeVector<SharedPtr<WorkItem> >::iterator i = workItems_.begin(); i != workItems_.end();)
		{
			if((*i)->completed_ && (*i)->priority_ >= priority)
			{
				if((*i)->sendEvent_)
				{
					//
				}

				ReturnToPool(*i);
				i = workItems_.erase(i);
			}
			else
				++i;
		}
	}

	void YumeWorkQueue::PurgePool()
	{
		unsigned currentSize = poolItems_.size();
		int difference = lastSize_ - currentSize;

		// Difference tolerance, should be fairly significant to reduce the pool size.
		for(unsigned i = 0; poolItems_.size() > 0 && difference > tolerance_ && i < (unsigned)difference; i++)
			poolItems_.erase(poolItems_.begin());

		lastSize_ = currentSize;
	}

	void YumeWorkQueue::ReturnToPool(SharedPtr<WorkItem>& item)
	{
		// Check if this was a pooled item and set it to usable
		if(item->pooled_)
		{
			// Reset the values to their defaults. This should 
			// be safe to do here as the completed event has
			// already been handled and this is part of the
			// internal pool.
			item->start_ = 0;
			item->end_ = 0;
			item->aux_ = 0;
			item->workFunction_ = 0;
			item->priority_ = M_MAX_UNSIGNED;
			item->sendEvent_ = false;
			item->completed_ = false;

			poolItems_.push_back(item);
		}
	}

	void YumeWorkQueue::HandleBeginFrame(YumeHash eventType,VariantMap& eventData)
	{
		// If no worker threads, complete low-priority work here
		if(threads_.empty() && !queue_.empty())
		{

			YumeHiresTimer timer;

			while(!queue_.empty() && timer.GetUSec(false) < maxNonThreadedWorkMs_ * 1000)
			{
				WorkItem* item = queue_.front();
				queue_.erase(queue_.begin());
				item->workFunction_(item,0);
				item->completed_ = true;
			}
		}

		// Complete and signal items down to the lowest priority
		PurgeCompleted(0);
		PurgePool();
	}

}
