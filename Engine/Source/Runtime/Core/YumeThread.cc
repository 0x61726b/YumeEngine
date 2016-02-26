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

#include "YumeThread.h"




namespace YumeEngine
{

#ifdef _WIN32

	DWORD WINAPI ThreadFunctionStatic(void* data)
	{
		YumeThreadWrapper* thread = static_cast<YumeThreadWrapper*>(data);
		thread->ThreadRunner();
		return 0;
	}

#else

	void* ThreadFunctionStatic(void* data)
	{
		YumeThreadWrapper* thread = static_cast<YumeThreadWrapper*>(data);
		thread->ThreadRunner();
		pthread_exit((void*)0);
		return 0;
	}

#endif

	ThreadID YumeThreadWrapper::mainThreadID;

	YumeThreadWrapper::YumeThreadWrapper(): shouldRun_(false), threadHandle(0)
	{
	}

	YumeThreadWrapper::~YumeThreadWrapper()
	{
		Stop();
	}

	bool YumeThreadWrapper::Run()
	{
		if(threadHandle)
			return false;

		shouldRun_ = true;

		threadHandle = new boost::thread(boost::bind(&YumeThreadWrapper::ThreadRunner,this));

		return threadHandle != 0;
	}

	void YumeThreadWrapper::Stop()
	{
		if(!threadHandle)
			return;

		shouldRun_ = false;

		threadHandle->interrupt();

		threadHandle = 0;
	}

	void YumeThreadWrapper::SetPriority(int pr)
	{
		if(!threadHandle)
			return;


#ifdef _WIN32
		if(threadHandle->native_handle())
			SetThreadPriority((HANDLE)threadHandle->native_handle(),pr);
#endif
#if defined(__linux__)
		pthread_t* thread = (pthread_t*)threadHandle->native_handle();
		if(thread)
			pthread_setschedprio(*thread,pr);
#endif
	}


	void YumeThreadWrapper::SetMainThread()
	{
		mainThreadID = GetCurrentThreadID();
	}

	ThreadID YumeThreadWrapper::GetCurrentThreadID()
	{
#ifdef _WIN32
		return GetCurrentThreadId();
#else
		return pthread_self();
#endif
	}

	bool YumeThreadWrapper::IsMainThread()
	{
		return GetCurrentThreadID() == mainThreadID;
	}
}
