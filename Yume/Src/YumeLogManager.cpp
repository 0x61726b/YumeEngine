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
#include "YumeLogManager.h"
#include <algorithm>
namespace YumeEngine
{
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	template<> YumeLogManager* Singleton<YumeLogManager>::msSingleton = 0;
	YumeLogManager* YumeLogManager::GetPtr(void)
	{
		return msSingleton;
	}
	YumeLogManager& YumeLogManager::Get(void)
	{
		assert(msSingleton);  return (*msSingleton);
	}
	//-----------------------------------------------------------------------
	YumeLogManager::YumeLogManager()
	{
		mDefaultLog = NULL;
	}
	//-----------------------------------------------------------------------
	YumeLogManager::~YumeLogManager()
	{

		// Destroy all logs
		LogList::iterator i;
		for (i = mLogs.begin(); i != mLogs.end(); ++i)
		{
			YumeAPIDelete i->second;
		}
	}
	//-----------------------------------------------------------------------
	YumeLog* YumeLogManager::createLog(const YumeString& name, bool defaultLog, bool debuggerOutput,
		bool suppressFileOutput)
	{

		YumeLog* newLog = YumeAPINew YumeLog(name, debuggerOutput, suppressFileOutput);

		if (!mDefaultLog || defaultLog)
		{
			mDefaultLog = newLog;
		}

		mLogs.insert(LogList::value_type(name, newLog));

		return newLog;
	}
	//-----------------------------------------------------------------------
	YumeLog* YumeLogManager::getDefaultLog()
	{
		return mDefaultLog;
	}
	//-----------------------------------------------------------------------
	YumeLog* YumeLogManager::setDefaultLog(YumeLog* newLog)
	{
		YumeLog* oldLog = mDefaultLog;
		mDefaultLog = newLog;
		return oldLog;
	}
	//-----------------------------------------------------------------------
	YumeLog* YumeLogManager::getLog(const YumeString& name)
	{

		LogList::iterator i = mLogs.find(name);
		if (i != mLogs.end())
			return i->second;
			/*OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Log not found. ", "LogManager::getLog");*/

		return nullptr;
	}
	//-----------------------------------------------------------------------
	void YumeLogManager::destroyLog(const YumeString& name)
	{
		LogList::iterator i = mLogs.find(name);
		if (i != mLogs.end())
		{
			if (mDefaultLog == i->second)
			{
				mDefaultLog = 0;
			}
			YumeAPIDelete i->second;
			mLogs.erase(i);
		}

		// Set another default log if this one removed
		if (!mDefaultLog && !mLogs.empty())
		{
			mDefaultLog = mLogs.begin()->second;
		}
	}
	//-----------------------------------------------------------------------
	void YumeLogManager::destroyLog(YumeLog* log)
	{
		destroyLog(log->getName());
	}
	//-----------------------------------------------------------------------
	void YumeLogManager::Log(const YumeString& message, LogMessageLevel lml, bool maskDebug)
	{

		if (mDefaultLog)
		{
			mDefaultLog->logMessage(message, lml, maskDebug);
		}
	}
	//-----------------------------------------------------------------------
	void YumeLogManager::setLogDetail(LoggingLevel ll)
	{

		if (mDefaultLog)
		{
			mDefaultLog->setLogDetail(ll);
		}
	}
	//---------------------------------------------------------------------
	YumeLog::Stream YumeLogManager::stream(LogMessageLevel lml, bool maskDebug)
	{

		if (mDefaultLog)
			return mDefaultLog->stream(lml, maskDebug);
		/*OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Default log not found. ", "LogManager::stream");*/
		return mDefaultLog->stream(lml, maskDebug);
	}
}