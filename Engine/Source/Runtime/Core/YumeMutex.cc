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
#include "YumeMutex.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace YumeEngine
{

#ifdef _WIN32

	Mutex::Mutex():
		handle_(new CRITICAL_SECTION)
	{
		InitializeCriticalSection((CRITICAL_SECTION*)handle_);
	}

	Mutex::~Mutex()
	{
		CRITICAL_SECTION* cs = (CRITICAL_SECTION*)handle_;
		DeleteCriticalSection(cs);
		delete cs;
		handle_ = 0;
	}

	void Mutex::Acquire()
	{
		EnterCriticalSection((CRITICAL_SECTION*)handle_);
	}

	bool Mutex::TryAcquire()
	{
		return TryEnterCriticalSection((CRITICAL_SECTION*)handle_) != FALSE;
	}

	void Mutex::Release()
	{
		LeaveCriticalSection((CRITICAL_SECTION*)handle_);
		ReleaseMutex(handle_);

	}

#else

	Mutex::Mutex():
		handle_(new pthread_mutex_t)
	{
		pthread_mutex_t* mutex = (pthread_mutex_t*)handle_;
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(mutex,&attr);
	}

	Mutex::~Mutex()
	{
		pthread_mutex_t* mutex = (pthread_mutex_t*)handle_;
		pthread_mutex_destroy(mutex);
		delete mutex;
		handle_ = 0;
	}

	void Mutex::Acquire()
	{
		pthread_mutex_lock((pthread_mutex_t*)handle_);
	}

	bool Mutex::TryAcquire()
	{
		return pthread_mutex_trylock((pthread_mutex_t*)handle_) == 0;
	}

	void Mutex::Release()
	{
		pthread_mutex_unlock((pthread_mutex_t*)handle_);
	}

#endif

	MutexLock::MutexLock(Mutex& mutex):
		mutex_(mutex)
	{
		mutex_.Acquire();
	}

	MutexLock::~MutexLock()
	{
		mutex_.Release();
	}
}
