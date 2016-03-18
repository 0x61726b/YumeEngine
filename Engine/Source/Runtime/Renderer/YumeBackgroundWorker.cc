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
		MutexLock lock(backgroundLoadMutex_);

		backgroundLoadQueue_.clear();
	}

	void YumeBackgroundWorker::ThreadRunner()
	{

	}


	bool YumeBackgroundWorker::QueueResource(YumeHash type,const YumeString& name,bool sendEventOnFailure,YumeResource* caller)
	{
		return true;
	}

	void YumeBackgroundWorker::WaitForResource(YumeHash type,const YumeString& resource)
	{
		
	}

	void YumeBackgroundWorker::FinishResources(int maxMs)
	{
		
	}

	unsigned YumeBackgroundWorker::GetNumQueuedResources() const
	{
		return 0;
	}

	void YumeBackgroundWorker::FinishBackgroundLoading(WorkItem& item)
	{

	}

}
