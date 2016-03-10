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
#include "YumeResource.h"

#include <boost/functional/hash.hpp>

namespace YumeEngine
{
	YumeResource::YumeResource():
		memoryUse_(0),
		asyncLoadState_(ASYNC_DONE)
	{
	}

	YumeResource::~YumeResource()
	{
	}

	bool YumeResource::Load(YumeFile& source)
	{
		SetAsyncLoadingState(YumeThreadWrapper::IsMainThread() ? ASYNC_DONE : ASYNC_LOADING);
		bool success = BeginLoad(source);
		if(success)
			success &= EndLoad();
		SetAsyncLoadingState(ASYNC_DONE);

		return success;
	}

	bool YumeResource::BeginLoad(YumeFile& source)
	{
		return false;
	}

	bool YumeResource::EndLoad()
	{
		return false;
	}


	bool YumeResource::Save(YumeFile& dest) const
	{
		
		return false;
	}

	void YumeResource::SetName(const YumeString& name)
	{
		boost::hash<YumeString> nameHash;
		name_ = name;
		nameHash_ = nameHash(name);
	}

	void YumeResource::SetMemoryUsage(unsigned size)
	{
		memoryUse_ = size;
	}

	void YumeResource::ResetUseTimer()
	{
		useTimer_.Reset();
	}

	void YumeResource::SetAsyncLoadingState(AsyncLoadState newState)
	{
		asyncLoadState_ = newState;
	}

	unsigned YumeResource::GetUseTimer()
	{
		
		//if(Refs() > 1)
		//{
		//	useTimer_.Reset();
		//	return 0;
		//}
		//else
		//	return useTimer_.GetMSec(false);

		return 0;
	}
}
