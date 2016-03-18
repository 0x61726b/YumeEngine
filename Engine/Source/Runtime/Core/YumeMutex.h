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
#ifndef __YumeMutex_h__
#define __YumeMutex_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{

	/// Operating system mutual exclusion primitive.
	class YumeAPIExport Mutex
	{
	public:
		/// Construct.
		Mutex();
		/// Destruct.
		~Mutex();

		/// Acquire the mutex. Block if already acquired.
		void Acquire();
		/// Try to acquire the mutex without locking. Return true if successful.
		bool TryAcquire();
		/// Release the mutex.
		void Release();

	private:
		/// Mutex handle.
		void* handle_;
	};

	/// Lock that automatically acquires and releases a mutex.
	class YumeAPIExport MutexLock
	{
	public:
		/// Construct and acquire the mutex.
		MutexLock(Mutex& mutex);
		/// Destruct. Release the mutex.
		~MutexLock();

	private:
		/// Prevent copy construction.
		MutexLock(const MutexLock& rhs);
		/// Prevent assignment.
		MutexLock& operator =(const MutexLock& rhs);

		/// Mutex reference.
		Mutex& mutex_;
	};

}


//----------------------------------------------------------------------------
#endif
