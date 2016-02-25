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
#ifndef __YumeBackgroundWorker_h__
#define __YumeBackgroundWorker_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Core/YumeThread.h"

#include <boost/thread/mutex.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeResource;
	class YumeResourceManager;

	struct WorkItem
	{
		boost::shared_ptr<YumeResource> resource_;
		
		YumeSet<std::pair<YumeHash,YumeHash> >::type dependencies_;
		YumeSet<std::pair<YumeHash,YumeHash> >::type dependents_;
		bool sendEventOnFailure_;
	};

	class YumeAPIExport YumeBackgroundWorker : public YumeThreadWrapper
	{
	public:
		YumeBackgroundWorker(YumeResourceManager* cache);

		~YumeBackgroundWorker();

		virtual void ThreadRunner();


		bool QueueResource(YumeHash type,const YumeString& name,bool sendEventOnFailure,YumeResource* caller);

		void WaitForResource(YumeHash type,YumeHash nameHash);

		void FinishResources(int maxMs);

		/// Return amount of resources in the load queue.
		unsigned GetNumQueuedResources() const;


	private:
		/// Finish one background loaded resource.
		void FinishBackgroundLoading(WorkItem& item);

		/// Resource cache.
		YumeResourceManager* owner_;
		/// Mutex for thread-safe access to the background load queue.
		mutable boost::mutex backgroundLoadMutex_;
		/// Resources that are queued for background loading.
		YumeMap<std::pair<YumeHash,YumeHash>,WorkItem>::type backgroundLoadQueue_;
	};
}


//----------------------------------------------------------------------------
#endif
