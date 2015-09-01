///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 Alperen Gezer

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : YumeLog.cpp
/// Date : 9.1.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

#include "YumeHeaders.h"
#include "YumeLog.h"

#include "YumeString.h"

namespace YumeEngine
{
	//-----------------------------------------------------------------------
	YumeLog::YumeLog(const YumeString& name, bool debuggerOuput, bool suppressFile) :
		mLogLevel(LL_NORMAL), mDebugOut(debuggerOuput),
		mSuppressFile(suppressFile), mTimeStamp(true), mLogName(name)
	{
		if (!mSuppressFile)
		{
			mLog.open(name.c_str());
		}
	}
	//-----------------------------------------------------------------------
	YumeLog::~YumeLog()
	{
		
			if (!mSuppressFile)
			{
				mLog.close();
			}
	}
	//-----------------------------------------------------------------------
	void YumeLog::logMessage(const YumeString& message, LogMessageLevel lml, bool maskDebug)
	{
		
			if ((mLogLevel + lml) >= OGRE_LOG_THRESHOLD)
			{
				bool skipThisMessage = false;
				for (mtLogListener::iterator i = mListeners.begin(); i != mListeners.end(); ++i)
					(*i)->messageLogged(message, lml, maskDebug, mLogName, skipThisMessage);

				if (!skipThisMessage)
				{

					if (mDebugOut && !maskDebug)
						std::cerr << message << std::endl;

					// Write time into log
					if (!mSuppressFile)
					{
						if (mTimeStamp)
						{
							struct tm *pTime;
							time_t ctTime; time(&ctTime);
							pTime = localtime(&ctTime);
							mLog << std::setw(2) << std::setfill('0') << pTime->tm_hour
								<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
								<< ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec
								<< ": ";
						}
						mLog << message << std::endl;

						// Flush stcmdream to ensure it is written (incase of a crash, we need log to be up to date)
						mLog.flush();
					}
				}
			}
	}

	//-----------------------------------------------------------------------
	void YumeLog::setTimeStampEnabled(bool timeStamp)
	{
		
			mTimeStamp = timeStamp;
	}

	//-----------------------------------------------------------------------
	void YumeLog::setDebugOutputEnabled(bool debugOutput)
	{
		
			mDebugOut = debugOutput;
	}

	//-----------------------------------------------------------------------
	void YumeLog::setLogDetail(LoggingLevel ll)
	{
	
			mLogLevel = ll;
	}

	//-----------------------------------------------------------------------
	void YumeLog::addListener(LogListener* listener)
	{
		
			mListeners.push_back(listener);
	}

	//-----------------------------------------------------------------------
	void YumeLog::removeListener(LogListener* listener)
	{
		
			mListeners.erase(std::find(mListeners.begin(), mListeners.end(), listener));
	}
	//---------------------------------------------------------------------
	YumeLog::Stream YumeLog::stream(LogMessageLevel lml, bool maskDebug)
	{
		return Stream(this, lml, maskDebug);

	}
}