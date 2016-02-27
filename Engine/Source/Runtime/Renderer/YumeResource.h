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
// File : <Filename> <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeResource_h__
#define __YumeResource_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeTimer.h"
#include "YumeThread.h"
#include "YumeBase.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeFile;
	
	/// Asynchronous loading state of a resource.
	enum AsyncLoadState
	{
		/// No async operation in progress.
		ASYNC_DONE = 0,
		/// Queued for asynchronous loading.
		ASYNC_QUEUED = 1,
		/// In progress of calling BeginLoad() in a worker thread.
		ASYNC_LOADING = 2,
		/// BeginLoad() succeeded. EndLoad() can be called in the main thread.
		ASYNC_SUCCESS = 3,
		/// BeginLoad() failed.
		ASYNC_FAIL = 4
	};

	class YumeAPIExport YumeResource : public YumeBase
	{
	public:
		YumeResource();
		virtual ~YumeResource();

		virtual bool Load(YumeFile& source);

		virtual bool BeginLoad(YumeFile& source);

		virtual bool EndLoad();

		virtual bool Save(YumeFile& source) const;

		void SetName(const YumeString& name);

		void SetMemoryUsage(unsigned size);

		void SetAsyncLoadingState(AsyncLoadState state);

		void ResetUseTimer();

		const YumeString& GetName() const { return name_; }

		/// Return name hash.
		std::size_t GetNameHash() const { return nameHash_; }

		/// Return memory use in bytes, possibly approximate.
		unsigned GetMemoryUse() const { return memoryUse_; }

		/// Return time since last use in milliseconds. If referred to elsewhere than in the resource cache, returns always zero.
		unsigned GetUseTimer();

		/// Return the asynchronous loading state.
		AsyncLoadState GetAsyncLoadState() const { return asyncLoadState_; }

	private:
		/// Name.
		YumeString name_;
		/// Name hash.
		std::size_t nameHash_;
		/// Last used timer.
		YumeLowresTimer useTimer_;
		/// Memory use in bytes.
		unsigned memoryUse_;
		/// Asynchronous loading state.
		AsyncLoadState asyncLoadState_;
	};
}


//----------------------------------------------------------------------------
#endif
