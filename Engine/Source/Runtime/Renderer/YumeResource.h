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
	
	
	enum AsyncLoadState
	{
		
		ASYNC_DONE = 0,
		
		ASYNC_QUEUED = 1,
		
		ASYNC_LOADING = 2,
		
		ASYNC_SUCCESS = 3,
		
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

		static YumeHash type_;
		static YumeHash GetTypeStatic() { return type_; }
		virtual YumeHash GetType() { return type_; }

		
		std::size_t GetNameHash() const { return nameHash_; }
		unsigned GetMemoryUse() const { return memoryUse_; }	
		unsigned GetUseTimer();

		AsyncLoadState GetAsyncLoadState() const { return asyncLoadState_; }

	private:
		
		YumeString name_;
		
		std::size_t nameHash_;
		
		YumeLowresTimer useTimer_;
		
		unsigned memoryUse_;
		
		AsyncLoadState asyncLoadState_;
	};
}


//----------------------------------------------------------------------------
#endif
