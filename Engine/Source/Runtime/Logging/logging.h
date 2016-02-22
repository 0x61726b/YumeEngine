//----------------------------------------------------------------------------
//Chiika Api
//Copyright (C) 2016  arkenthera
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
#pragma once
#ifndef __logging_h__
#define __logging_h__
//----------------------------------------------------------------------------

#include <sstream>
#include <list>

#if !YUME_NO_LOGGING_MACROS
#  if defined(__GNUC__)
#    define YUMELOG_FUNCTION() __PRETTY_FUNCTION__
#  elif defined(_MSC_VER)
#    define YUMELOG_FUNCTION() __FUNCTION__
#  else
#    define YUMELOG_FUNCTION() __func__
#  endif
#  define YUMELOG_LOG_BODY(type, src, msg) \
    do { \
        std::ostringstream os; \
        os << msg; \
        YumeEngine::Log:: type (src, os.str(), __FILE__, __LINE__, YUMELOG_FUNCTION()); \
		    } while(0)
#endif

#if !CHIIKA_NO_LOGGING_MACROS
#  define YUMELOG_TRACE(msg) YUMELOG_LOG_BODY(trace, "Src", msg)
#  define YUMELOG_DEBUG(msg) YUMELOG_LOG_BODY(debug, "Src", msg)
#  define YUMELOG_INFO(msg) YUMELOG_LOG_BODY(info, "Src", msg)
#  define YUMELOG_WARN(msg) YUMELOG_LOG_BODY(warn, "Src", msg)
#  define YUMELOG_ERROR(msg) YUMELOG_LOG_BODY(error, "Src", msg)
#  define YUMELOG_FATAL(msg) YUMELOG_LOG_BODY(fatal, "Src", msg)
#else 
#  define YUMELOG_TRACE(src, msg) 
#  define YUMELOG_DEBUG(src, msg)
#  define YUMELOG_INFO(src, msg)
#  define YUMELOG_WARN(src, msg)
#  define YUMELOG_ERROR(src, msg)
#  define YUMELOG_FATAL(src, msg)
#endif
//----------------------------------------------------------------------------
namespace YumeEngine
{
	namespace Log
	{
		void InitLogging(const char* loc);
		void StopLogging();

		void YumeAPIExport trace(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
		void YumeAPIExport debug(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
		void YumeAPIExport info(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
		void YumeAPIExport warn(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
		void YumeAPIExport error(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);
		void YumeAPIExport fatal(const std::string& src, const std::string& msg, const char *file, int line, const char *fn);

		enum LogLevel {
			// All log messages, no matter how small
			LogLevel_Trace = 0x01,
			// Debug level log messages - messages you won't care about unless you're debugging
			LogLevel_Debug = 0x02,
			// Informational log messages - not critical to know, but you might care
			LogLevel_Info = 0x04,
			// Only log warning and worse messages
			LogLevel_Warn = 0x08,
			// Only log messages that are actual errors
			LogLevel_Error = 0x10
		};
	}
}
//----------------------------------------------------------------------------
#endif