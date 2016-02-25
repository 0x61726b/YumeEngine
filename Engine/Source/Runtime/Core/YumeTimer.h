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
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#ifndef __YumeTimer_h__
#define __YumeTimer_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include <boost/timer.hpp>

//----------------------------------------------------------------------------
namespace YumeEngine
{
	/// Low-resolution operating system timer.
	class YumeAPIExport YumeLowresTimer
	{
	public:
		/// Construct. Get the starting clock value.
		YumeLowresTimer();

		/// Return elapsed milliseconds and optionally reset.
		unsigned GetMSec(bool reset);
		/// Reset the timer.
		void Reset();

	private:
		/// Starting clock value in milliseconds.
		unsigned startTime_;
	};

	class YumeAPIExport YumeHiresTimer
	{
		friend class YumeTime;

	public:
		/// Construct. Get the starting high-resolution clock value.
		YumeHiresTimer();

		/// Return elapsed microseconds and optionally reset.
		long long GetUSec(bool reset);
		/// Reset the timer.
		void Reset();

		/// Return if high-resolution timer is supported.
		static bool IsSupported() { return supported; }

		/// Return high-resolution timer frequency if supported.
		static long long GetFrequency() { return frequency; }

	private:
		/// Starting clock value in CPU ticks.
		long long startTime_;

		/// High-resolution timer support flag.
		static bool supported;
		/// High-resolution timer frequency.
		static long long frequency;
	};
	class YumeAPIExport YumeTime
	{
	public:
		YumeTime();
		~YumeTime();

		void BeginFrame(float dt);
		void EndFrame();

		static void Sleep(unsigned mSce);
		int GetFrameNumber() const { return frameNumber_; }
		float GetTimeStep() const { return timeStep_; }

		YumeString GetTimeStamp();
		double GetElapsedTime();
	protected:
		int frameNumber_;
		double timeStep_;
		boost::timer totalTime_;

	};
}

//----------------------------------------------------------------------------
#endif
