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
/// File : YumeRendererCapabilities.h
/// Date : 9.1.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeLogManager_h__
#define __YumeLogManager_h__
//---------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeLog.h"
#include "YumeSingleton.h"
#include "YumeString.h"
//---------------------------------------------------------------------------------


namespace YumeEngine
{
	class YumeAPIExport YumeLogManager : public Singleton<YumeLogManager>, public LogObjAlloc
	{
	protected:
		typedef YumeMap<YumeString, YumeLog*>::type	LogList;

		/// A list of all the logs the manager can access
		LogList mLogs;

		/// The default log to which output is done
		YumeLog* mDefaultLog;

	public:
		YumeLogManager();
		~YumeLogManager();

		/** Creates a new log with the given name.
		@param
		name The name to give the log e.g. 'Ogre.log'
		@param
		defaultLog If true, this is the default log output will be
		sent to if the generic logging methods on this class are
		used. The first log created is always the default log unless
		this parameter is set.
		@param
		debuggerOutput If true, output to this log will also be
		routed to the debugger's output window.
		@param
		suppressFileOutput If true, this is a logical rather than a physical
		log and no file output will be written. If you do this you should
		register a LogListener so log output is not lost.
		*/
		YumeLog* createLog(const YumeString& name, bool defaultLog = false, bool debuggerOutput = true,
			bool suppressFileOutput = false);

		/** Retrieves a log managed by this class.
		*/
		YumeLog* getLog(const YumeString& name);

		/** Returns a pointer to the default log.
		*/
		YumeLog* getDefaultLog();

		/** Closes and removes a named log. */
		void destroyLog(const YumeString& name);
		/** Closes and removes a log. */
		void destroyLog(YumeLog* log);

		/** Sets the passed in log as the default log.
		@return The previous default log.
		*/
		YumeLog* setDefaultLog(YumeLog* newLog);

		/** Log a message to the default log.
		*/
		void logMessage(const YumeString& message, LogMessageLevel lml = LML_NORMAL,
			bool maskDebug = false);

		/** Log a message to the default log (signature for backward compatibility).
		*/
		void logMessage(LogMessageLevel lml, const YumeString& message,
			bool maskDebug = false) {
			logMessage(message, lml, maskDebug);
		}

		/** Get a stream on the default log. */
		YumeLog::Stream stream(LogMessageLevel lml = LML_NORMAL,
			bool maskDebug = false);

		/** Sets the level of detail of the default log.
		*/
		void setLogDetail(LoggingLevel ll);
		/** Override standard Singleton retrieval.
		@remarks
		Why do we do this? Well, it's because the Singleton
		implementation is in a .h file, which means it gets compiled
		into anybody who includes it. This is needed for the
		Singleton template to work, but we actually only want it
		compiled into the implementation of the class based on the
		Singleton, not all of them. If we don't change this, we get
		link errors when trying to use the Singleton-based class from
		an outside dll.
		@par
		This method just delegates to the template version anyway,
		but the implementation stays in this single compilation unit,
		preventing link errors.
		*/
		static YumeLogManager& Get(void);
		/** Override standard Singleton retrieval.
		@remarks
		Why do we do this? Well, it's because the Singleton
		implementation is in a .h file, which means it gets compiled
		into anybody who includes it. This is needed for the
		Singleton template to work, but we actually only want it
		compiled into the implementation of the class based on the
		Singleton, not all of them. If we don't change this, we get
		link errors when trying to use the Singleton-based class from
		an outside dll.
		@par
		This method just delegates to the template version anyway,
		but the implementation stays in this single compilation unit,
		preventing link errors.
		*/
		static YumeLogManager* GetPtr(void);

	};
}

#endif