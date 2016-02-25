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
#include "YumeBackgroundWorker.h"
#include "YumeResourceManager.h"

#include "Core/YumeTimer.h"


#include "Logging/logging.h"

namespace YumeEngine
{
	YumeBackgroundWorker::YumeBackgroundWorker(YumeResourceManager* owner):
		owner_(owner)
	{
	}

	YumeBackgroundWorker::~YumeBackgroundWorker()
	{
		boost::mutex::scoped_lock lock(backgroundLoadMutex_);

		backgroundLoadQueue_.clear();
	}

	void YumeBackgroundWorker::ThreadRunner()
	{
		while(shouldRun_)
		{
			backgroundLoadMutex_.lock();

			YumeMap<std::pair<YumeHash,YumeHash>,WorkItem>::iterator It = backgroundLoadQueue_.begin();

			while(It != backgroundLoadQueue_.end())
			{
				if(It->second.resource_->GetAsyncLoadState() == ASYNC_QUEUED)
					break;
				else
					++It;
			}

			if(It == backgroundLoadQueue_.end())
			{
				// No resources to load found
				backgroundLoadMutex_.unlock();
				YumeTime::Sleep(5);
			}
			else
			{
				WorkItem& item = It->second;
				YumeResource* resource = item.resource_.get();
				// We can be sure that the item is not removed from the queue as long as it is in the
				// "queued" or "loading" state
				backgroundLoadMutex_.unlock();

				bool success = false;
				boost::shared_ptr<YumeFile> file = owner_->GetFile(resource->GetName(),item.sendEventOnFailure_);
				if(file)
				{
					resource->SetAsyncLoadingState(ASYNC_LOADING);
					success = resource->BeginLoad(*file);
				}

				// Process dependencies now
				// Need to lock the queue again when manipulating other entries
				boost::hash<YumeString> hasher;

				std::pair<YumeHash,YumeHash> key = std::make_pair(hasher("Resource"),resource->GetNameHash());
				backgroundLoadMutex_.lock();
				if(item.dependents_.size())
				{
					for(YumeSet<std::pair<YumeHash,YumeHash> >::iterator i = item.dependents_.begin();
						i != item.dependents_.end(); ++i)
					{
						YumeMap<std::pair<YumeHash,YumeHash>,WorkItem>::iterator j = backgroundLoadQueue_.find(*i);
						if(j != backgroundLoadQueue_.end())
							j->second.dependencies_.erase(key);
					}

					item.dependents_.clear();
				}

				resource->SetAsyncLoadingState(success ? ASYNC_SUCCESS : ASYNC_FAIL);
				backgroundLoadMutex_.unlock();
			}
		}
	}


	bool YumeBackgroundWorker::QueueResource(YumeHash type,const YumeString& name,bool sendEventOnFailure,YumeResource* caller)
	{
		boost::hash<YumeString> hasher;
		YumeHash nameHash = hasher(name);

		std::pair<YumeHash,YumeHash> key = std::make_pair(type,nameHash);

		boost::mutex::scoped_lock lock(backgroundLoadMutex_);

		// Check if already exists in the queue
		if(backgroundLoadQueue_.find(key) != backgroundLoadQueue_.end())
			return false;

		WorkItem& item = backgroundLoadQueue_[key];
		item.sendEventOnFailure_ = sendEventOnFailure;

		if(!item.resource_)
		{
			YUMELOG_ERROR("Could not load unknown resource type " << type);


			backgroundLoadQueue_.erase(key);
			return false;
		}

		YUMELOG_ERROR("Background loading resource " + name);

		item.resource_->SetName(name);
		item.resource_->SetAsyncLoadingState(ASYNC_QUEUED);

		// If this is a resource calling for the background load of more resources, mark the dependency as necessary
		if(caller)
		{
			boost::hash<YumeString> hasher;
			std::pair<YumeHash,YumeHash> callerKey = std::make_pair(hasher("Resource"),caller->GetNameHash());
			YumeMap<std::pair<YumeHash,YumeHash>,WorkItem>::iterator j = backgroundLoadQueue_.find(callerKey);
			if(j != backgroundLoadQueue_.end())
			{
				WorkItem& callerItem = j->second;
				item.dependents_.insert(callerKey);
				callerItem.dependencies_.insert(key);
			}
			else
				YUMELOG_WARN("Resource " + caller->GetName() +
				" requested for a background loaded resource but was not in the background load queue");
		}

		// Start the background loader thread now
		if(!IsStarted())
			Run();

		return true;
	}

	void YumeBackgroundWorker::WaitForResource(YumeHash type,YumeHash nameHash)
	{
		backgroundLoadMutex_.lock();

		// Check if the resource in question is being background loaded
		std::pair<YumeHash,YumeHash> key = std::make_pair(type,nameHash);
		YumeMap<std::pair<YumeHash,YumeHash>,WorkItem>::iterator i = backgroundLoadQueue_.find(key);
		if(i != backgroundLoadQueue_.end())
		{
			backgroundLoadMutex_.unlock();

			{
				YumeResource* resource = i->second.resource_.get();
				YumeHiresTimer waitTimer;
				bool didWait = false;

				for(;;)
				{
					unsigned numDeps = i->second.dependencies_.size();
					AsyncLoadState state = resource->GetAsyncLoadState();
					if(numDeps > 0 || state == ASYNC_QUEUED || state == ASYNC_LOADING)
					{
						didWait = true;
						YumeTime::Sleep(1);
					}
					else
						break;
				}

				if(didWait)
					YUMELOG_DEBUG("Waited " + std::to_string(waitTimer.GetUSec(false) / 1000) + " ms for background loaded resource " +
					resource->GetName());
			}

			// This may take a long time and may potentially wait on other resources, so it is important we do not hold the mutex during this
			FinishBackgroundLoading(i->second);

			backgroundLoadMutex_.lock();
			backgroundLoadQueue_.erase(i);
			backgroundLoadMutex_.unlock();
		}
		else
			backgroundLoadMutex_.unlock();
	}

	void YumeBackgroundWorker::FinishResources(int maxMs)
	{
		if(IsStarted())
		{
			YumeHiresTimer timer;

			backgroundLoadMutex_.lock();

			for(YumeMap<std::pair<YumeHash,YumeHash>,WorkItem>::iterator i = backgroundLoadQueue_.begin();
				i != backgroundLoadQueue_.end();)
			{
				YumeResource* resource = i->second.resource_.get();
				unsigned numDeps = i->second.dependencies_.size();
				AsyncLoadState state = resource->GetAsyncLoadState();
				if(numDeps > 0 || state == ASYNC_QUEUED || state == ASYNC_LOADING)
					++i;
				else
				{
					// Finishing a resource may need it to wait for other resources to load, in which case we can not
					// hold on to the mutex
					backgroundLoadMutex_.unlock();
					FinishBackgroundLoading(i->second);
					backgroundLoadMutex_.lock();
					i = backgroundLoadQueue_.erase(i);
				}

				// Break when the time limit passed so that we keep sufficient FPS
				if(timer.GetUSec(false) >= maxMs * 1000)
					break;
			}

			backgroundLoadMutex_.unlock();
		}
	}

	unsigned YumeBackgroundWorker::GetNumQueuedResources() const
	{
		boost::mutex::scoped_lock lock(backgroundLoadMutex_);
		return backgroundLoadQueue_.size();
	}

	void YumeBackgroundWorker::FinishBackgroundLoading(WorkItem& item)
	{
		YumeResource* resource = item.resource_.get();

		bool success = resource->GetAsyncLoadState() == ASYNC_SUCCESS;
		// If BeginLoad() phase was successful, call EndLoad() and get the final success/failure result
		if(success)
		{
			YUMELOG_INFO("Finishing background loaded resource " + resource->GetName());
			success = resource->EndLoad();

#ifdef URHO3D_PROFILING
			if(profiler)
				profiler->EndBlock();
#endif
		}
		resource->SetAsyncLoadingState(ASYNC_DONE);

		// Store to the cache; use same mechanism as for manual resources
		if(success)
			owner_->AddManualResource(resource);
	}

}
