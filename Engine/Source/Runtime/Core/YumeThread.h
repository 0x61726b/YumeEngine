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
#ifndef __YumeThread_h__
#define __YumeThread_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"

#ifndef _WIN32
#include <pthread.h>
typedef pthread_t ThreadID;
#else
typedef unsigned ThreadID;
#endif


#include <boost/thread.hpp>
//----------------------------------------------------------------------------
namespace YumeEngine
{
	//Just wraps boost::thread and few utility functions
	class YumeAPIExport YumeThreadWrapper
	{
	public:
		YumeThreadWrapper();

		virtual ~YumeThreadWrapper();

		virtual void ThreadRunner() = 0;

		bool Run();

		void Stop();

		void SetPriority(int priority);

		static void SetMainThread();

		static ThreadID GetCurrentThreadID();
		/// Return whether is executing in the main thread.
		static bool IsMainThread();

		bool IsStarted() const { return threadHandle != 0; }

	protected:
		/// Thread handle.
		boost::thread* threadHandle;
		/// Running flag.
		volatile bool shouldRun_;

		/// Main thread's thread ID.
		static ThreadID mainThreadID;
	};
}


//----------------------------------------------------------------------------
#endif
