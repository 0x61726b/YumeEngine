//----------------------------------------------------------------------------
//Chiika Api
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
//authors:arkenthera
//Date: 2.16.2016
//----------------------------------------------------------------------------
#include "YumeHeaders.h"


#include <boost/scoped_ptr.hpp>
#include "Logging/logging.h"
#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/initializer.h"
#include "log4cplus/win32consoleappender.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/loglevel.h"
#include "log4cplus/layout.h"
#include "log4cplus/ndc.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/nullappender.h"

#include "Engine/YumeEngine.h"
#include "Core/YumeEnvironment.h"

static log4cplus::LogLevel translate_logLevel(YumeEngine::Log::LogLevel ll);

static bool loggingEnabled = false;

void YumeEngine::Log::InitLogging(const char* loc)
{
	loggingEnabled = true;

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
	log4cplus::SharedAppenderPtr debugAppender(new log4cplus::Win32ConsoleAppender());
	debugAppender->setName(LOG4CPLUS_TEXT("First"));
	debugAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::SimpleLayout()));
	log4cplus::Logger::getRoot().addAppender(debugAppender);
#else
	log4cplus::SharedAppenderPtr debugAppender(new log4cplus::ConsoleAppender());
	debugAppender->setName(LOG4CPLUS_TEXT("ConsoleAppender"));
	debugAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::TTCCLayout()));
	log4cplus::Logger::getRoot().addAppender(debugAppender);
#endif

	std::string strModulePath = std::string(loc);
	const size_t cSize = strlen(loc);
	std::wstring wstrModulePath(cSize, L'#');
	mbstowcs(&wstrModulePath[0], loc, cSize);

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
	log4cplus::SharedAppenderPtr fileAppender(new log4cplus::RollingFileAppender(wstrModulePath));
#else
	log4cplus::SharedAppenderPtr fileAppender(new log4cplus::RollingFileAppender(strModulePath));
#endif

	fileAppender->setName(LOG4CPLUS_TEXT("Second"));
	fileAppender->setLayout(
		std::unique_ptr<log4cplus::Layout>(
		new log4cplus::PatternLayout(LOG4CPLUS_TEXT("[%-5p][%D{%Y/%m/%d %H:%M:%S:%q}][%t] %m%n"))));
	log4cplus::Logger::getRoot().addAppender(fileAppender);

}

void YumeEngine::Log::StopLogging()
{
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Yume"));
	logger.shutdown();

	ToggleLogging(false);
	

	
}
void YumeEngine::Log::ToggleLogging(bool b)
{
	loggingEnabled = b;
}

void YumeEngine::Log::trace(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
	if(loggingEnabled)
		LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Yume")),
			file << ":" << line << " - " << fn << " - " << msg.c_str());
}
void YumeEngine::Log::debug(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
	if(loggingEnabled)
		LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Yume")),
			fn << " - " << msg.c_str());
}
void YumeEngine::Log::info(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
	if(loggingEnabled)
		LOG4CPLUS_INFO(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Yume")),
			fn << " - " << msg.c_str());
}
void YumeEngine::Log::warn(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
	if(loggingEnabled)
		LOG4CPLUS_WARN(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Yume")),
			fn << " - " << msg.c_str());
}
void YumeEngine::Log::error(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
	if(loggingEnabled)
		LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Yume")),
			file << ":" << line << " - " << fn << " - " << msg.c_str());
}
void YumeEngine::Log::fatal(const std::string& src, const std::string& msg, const char *file, int line, const char *fn)
{
	if(loggingEnabled)
		LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Yume")),
			file << ":" << line << " - " << fn << " - " << msg.c_str());
}

static log4cplus::LogLevel translate_logLevel(YumeEngine::Log::LogLevel ll){

	switch (ll) {
	case(YumeEngine::Log::LogLevel_Trace) :
		return log4cplus::TRACE_LOG_LEVEL;
	case(YumeEngine::Log::LogLevel_Debug) :
		return log4cplus::DEBUG_LOG_LEVEL;
	case(YumeEngine::Log::LogLevel_Info) :
		return log4cplus::INFO_LOG_LEVEL;
	case(YumeEngine::Log::LogLevel_Warn) :
		return log4cplus::WARN_LOG_LEVEL;
	case(YumeEngine::Log::LogLevel_Error) :
		return log4cplus::ERROR_LOG_LEVEL;
	default:
		return log4cplus::WARN_LOG_LEVEL;
	}
}