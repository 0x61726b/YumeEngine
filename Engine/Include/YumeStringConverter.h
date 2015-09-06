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
#ifndef __YumeStringConverter_h__
#define __YumeStringConverter_h__
//---------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeStringVector.h"

namespace YumeEngine
{
	class YumeAPIExport YumeStringConverter
	{
	public:
		/** Converts an int to a String. */
		static YumeString toString(int val, unsigned short width = 0,
			char fill = ' ',
			std::ios::fmtflags flags = std::ios::fmtflags(0));
#if YUME_PLATFORM != YUME_PLATFORM_NACL &&  ( YUME_SYSTEM_ARCHITECTURE == YUME_SYSTEM_ARCH_64 || YUME_PLATFORM == YUME_PLATFORM_APPLE || YUME_PLATFORM == YUME_PLATFORM_APPLE_IOS )
		/** Converts an unsigned int to a String. */
		static YumeString toString(unsigned int val,
			unsigned short width = 0, char fill = ' ',
			std::ios::fmtflags flags = std::ios::fmtflags(0));
		/** Converts a size_t to a String. */
		static YumeString toString(size_t val,
			unsigned short width = 0, char fill = ' ',
			std::ios::fmtflags flags = std::ios::fmtflags(0));
#if YUME_COMPILER == YUME_COMPILER_MSVC
		/** Converts an unsigned long to a String. */
		static YumeString toString(unsigned long val,
			unsigned short width = 0, char fill = ' ',
			std::ios::fmtflags flags = std::ios::fmtflags(0));

#endif
#else
		/** Converts a size_t to a String. */
		static YumeString toString(size_t val,
			unsigned short width = 0, char fill = ' ',
			std::ios::fmtflags flags = std::ios::fmtflags(0));
		/** Converts an unsigned long to a String. */
		static YumeString toString(unsigned long val,
			unsigned short width = 0, char fill = ' ',
			std::ios::fmtflags flags = std::ios::fmtflags(0));
#endif
		/** Converts a long to a String. */
		static YumeString toString(long val,
			unsigned short width = 0, char fill = ' ',
			std::ios::fmtflags flags = std::ios::fmtflags(0));
		/** Converts a boolean to a String.
		@param yesNo If set to true, result is 'yes' or 'no' instead of 'true' or 'false'
		*/
		static YumeString toString(bool val, bool yesNo = false);

		static int parseInt(const YumeString& val, int defaultValue = 0);
		/** Converts a String to a whole number.
		@return
		0.0 if the value could not be parsed, otherwise the numeric version of the String.
		*/
		static unsigned int parseUnsignedInt(const YumeString& val, unsigned int defaultValue = 0);
		/** Converts a String to a whole number.
		@return
		0.0 if the value could not be parsed, otherwise the numeric version of the String.
		*/
		static long parseLong(const YumeString& val, long defaultValue = 0);
		/** Converts a String to a whole number.
		@return
		0.0 if the value could not be parsed, otherwise the numeric version of the String.
		*/
		static unsigned long parseUnsignedLong(const YumeString& val, unsigned long defaultValue = 0);
		/** Converts a String to size_t.
		@return
		defaultValue if the value could not be parsed, otherwise the numeric version of the String.
		*/
		static size_t parseSizeT(const YumeString& val, size_t defaultValue = 0);
		/** Converts a String to a boolean.
		@remarks
		Returns true if case-insensitive match of the start of the string
		matches "true", "yes" or "1", false otherwise.
		*/
		static bool parseBool(const YumeString& val, bool defaultValue = 0);
	};
}

//---------------------------------------------------------------------------------
#endif
//~End of __YumeRendererCapabilities_h__