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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeTimer.h"

#include <ctime>

#include <boost/algorithm/string/replace.hpp>
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

namespace YumeEngine
{
	bool YumeHiresTimer::supported(false);
	long long YumeHiresTimer::frequency(1000);

	YumeTime::YumeTime():
		frameNumber_(0),
		timeStep_(0)
	{
#ifdef _WIN32
		LARGE_INTEGER frequency;
		if(QueryPerformanceFrequency(&frequency))
		{
			YumeHiresTimer::frequency = frequency.QuadPart;
			YumeHiresTimer::supported = true;
		}
#else
		HiresTimer::frequency = 1000000;
		HiresTimer::supported = true;
#endif
	}

	YumeTime::~YumeTime()
	{

	}

	void YumeTime::BeginFrame(float dt)
	{
		timeStep_ = dt;

		++frameNumber_;
		if(!frameNumber_)
			++frameNumber_;


	}

	void YumeTime::EndFrame()
	{

	}

	double YumeTime::GetElapsedTime()
	{
		return totalTime_.elapsed();
	}

	YumeString YumeTime::GetTimeStamp()
	{
		time_t sysTime;
		time(&sysTime);
		const char* dateTime = ctime(&sysTime);
		YumeString dt(dateTime);
		boost::replace_all(dt,"\n","");
		return dt;
	}

	void YumeTime::Sleep(unsigned mSec)
	{
#ifdef _WIN32
		::Sleep(mSec);
#else
		usleep(mSec * 1000);
#endif
	}

	static unsigned Tick()
	{
#ifdef _WIN32
		return (unsigned)timeGetTime();
#else
		struct timeval time;
		gettimeofday(&time,NULL);
		return (unsigned)(time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
	}

	static long long HiresTick()
	{
#ifdef _WIN32
		if(YumeHiresTimer::IsSupported())
		{
			LARGE_INTEGER counter;
			QueryPerformanceCounter(&counter);
			return counter.QuadPart;
	}
		else
			return timeGetTime();
#else
		struct timeval time;
		gettimeofday(&time,NULL);
		return time.tv_sec * 1000000LL + time.tv_usec;
#endif
	}

	YumeHiresTimer::YumeHiresTimer()
	{
		Reset();
	}

	long long YumeHiresTimer::GetUSec(bool reset)
	{
		long long currentTime = HiresTick();
		long long elapsedTime = currentTime - startTime_;

		// Correct for possible weirdness with changing internal frequency
		if(elapsedTime < 0)
			elapsedTime = 0;

		if(reset)
			startTime_ = currentTime;

		return (elapsedTime * 1000000LL) / frequency;
	}

	void YumeHiresTimer::Reset()
	{
		startTime_ = HiresTick();
	}
}
