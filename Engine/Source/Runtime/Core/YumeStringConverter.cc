///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 arkenthera

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
/// File : YumeStringConveter.cpp
/// Date : 9.1.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

#include "YumeHeaders.h"
#include "Core/YumeStringConverter.h"

namespace YumeEngine
{

	YumeString YumeStringConverter::toString(int val,
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		YumeStringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
#if YUME_PLATFORM != YUME_PLATFORM_NACL &&  ( YUME_SYSTEM_ARCHITECTURE == YUME_SYSTEM_ARCH_64 || YUME_PLATFORM == YUME_PLATFORM_APPLE || YUME_PLATFORM == YUME_PLATFORM_APPLE_IOS )
	YumeString YumeStringConverter::toString(unsigned int val,
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		YumeStringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	YumeString YumeStringConverter::toString(size_t val,
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		YumeStringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
#if YUME_COMPILER == YUME_COMPILER_MSVC
	//-----------------------------------------------------------------------
	YumeString YumeStringConverter::toString(unsigned long val,
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		YumeStringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

#endif
	//-----------------------------------------------------------------------
#else
	YumeString YumeStringConverter::toString(size_t val,
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		YumeStringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	YumeString YumeStringConverter::toString(unsigned long val,
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		YumeStringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
#endif
	YumeString YumeStringConverter::toString(long val,
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		YumeStringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	YumeString YumeStringConverter::toString(bool val, bool yesNo)
	{
		if (val)
		{
			if (yesNo)
			{
				return "yes";
			}
			else
			{
				return "true";
			}
		}
		else
			if (yesNo)
			{
				return "no";
			}
			else
			{
				return "false";
			}
	}

	//-----------------------------------------------------------------------
	int YumeStringConverter::parseInt(const YumeString& val, int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		YumeStringStream str(val);
		int ret = defaultValue;
		if (!(str >> ret))
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	unsigned int YumeStringConverter::parseUnsignedInt(const YumeString& val, unsigned int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		YumeStringStream str(val);
		unsigned int ret = defaultValue;
		if (!(str >> ret))
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	long YumeStringConverter::parseLong(const YumeString& val, long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		YumeStringStream str(val);
		long ret = defaultValue;
		if (!(str >> ret))
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	unsigned long YumeStringConverter::parseUnsignedLong(const YumeString& val, unsigned long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		YumeStringStream str(val);
		unsigned long ret = defaultValue;
		if (!(str >> ret))
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	size_t YumeStringConverter::parseSizeT(const YumeString& val, size_t defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		YumeStringStream str(val);
		size_t ret = defaultValue;
		if (!(str >> ret))
			return defaultValue;

		return ret;
	}
	//-----------------------------------------------------------------------
	bool YumeStringConverter::parseBool(const YumeString& val, bool defaultValue)
	{
		if ((StringUtil::startsWith(val, "true") || StringUtil::startsWith(val, "yes")
			|| StringUtil::startsWith(val, "1")))
			return true;
		else if ((StringUtil::startsWith(val, "false") || StringUtil::startsWith(val, "no")
			|| StringUtil::startsWith(val, "0")))
			return false;
		else
			return defaultValue;
	}
	//-----------------------------------------------------------------------
}