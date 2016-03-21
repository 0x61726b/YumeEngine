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
#include "Math/YumeHash.h"
#include "YumeWorkQueue.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeResource;
	class YumeResourceManager;

	class YumeAPIExport YumeBackgroundWorker : public YumeThreadWrapper,public RefCounted
	{
	public:
		YumeBackgroundWorker(YumeResourceManager* cache);

		~YumeBackgroundWorker();

		virtual void ThreadRunner();


		bool QueueResource(YumeHash type,const YumeString& name,bool sendEventOnFailure,YumeResource* caller);

		void WaitForResource(YumeHash type,const YumeString& resource);

		void FinishResources(int maxMs);

		
		unsigned GetNumQueuedResources() const;


	private:
		
		void FinishBackgroundLoading(WorkItem& item);

		
		YumeResourceManager* owner_;
		
		mutable Mutex backgroundLoadMutex_;
		
		YumeMap<Pair<YumeHash,YumeHash>,WorkItem>::type backgroundLoadQueue_;
	};
}


//----------------------------------------------------------------------------
#endif
