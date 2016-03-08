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
#include "YumeHash.h"
#include "YumeMath.h"


namespace YumeEngine
{

	const YumeHash YumeHash::ZERO;

	YumeHash::YumeHash(const char* str):
		value_(Calculate(str))
	{
	}

	YumeHash::YumeHash(const YumeString& str) :
		value_(Calculate(str.c_str()))
	{
	}

	unsigned YumeHash::Calculate(const char* str)
	{
		unsigned hash = 0;

		if(!str)
			return hash;

		while(*str)
		{
			// Perform the actual hashing as case-insensitive
			char c = *str;
			hash = SDBMHash(hash,(unsigned char)tolower(c));
			++str;
		}

		return hash;
	}

	YumeString YumeHash::ToString() const
	{
		char tempBuffer[CONVERSION_BUFFER_LENGTH];
		sprintf(tempBuffer,"%08X",value_);
		return YumeString(tempBuffer);
	}

}
