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
#ifndef __YumeHash_h__
#define __YumeHash_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{


	class YumeAPIExport YumeHash
	{
	public:

		YumeHash():
			value_(0)
		{
		}


		YumeHash(const YumeHash& rhs):
			value_(rhs.value_)
		{
		}


		explicit YumeHash(unsigned value):
			value_(value)
		{
		}


		YumeHash(const char* str);

		YumeHash(const YumeString& str);


		YumeHash& operator =(const YumeHash& rhs)
		{
			value_ = rhs.value_;
			return *this;
		}


		YumeHash operator +(const YumeHash& rhs) const
		{
			YumeHash ret;
			ret.value_ = value_ + rhs.value_;
			return ret;
		}


		YumeHash& operator +=(const YumeHash& rhs)
		{
			value_ += rhs.value_;
			return *this;
		}

		// Test for equality with another hash.
		bool operator ==(const YumeHash& rhs) const { return value_ == rhs.value_; }
		bool operator !=(const YumeHash& rhs) const { return value_ != rhs.value_; }
		bool operator <(const YumeHash& rhs) const { return value_ < rhs.value_; }
		bool operator >(const YumeHash& rhs) const { return value_ > rhs.value_; }

		operator bool() const { return value_ != 0; }
		unsigned Value() const { return value_; }

		YumeString ToString() const;

		unsigned ToHash() const { return value_; }
		static unsigned Calculate(const char* str);
		static const YumeHash ZERO;
	private:

		unsigned value_;
	};
}

namespace std
{
	template <> 
	struct hash<YumeEngine::YumeHash>
	{
		size_t operator()(const YumeEngine::YumeHash& h) const 
			{ return  h.ToHash(); }
	};
}
namespace eastl
{
	template <> 
	struct hash<YumeEngine::YumeHash>
	{
		size_t operator()(const YumeEngine::YumeHash& h) const 
			{ return  h.ToHash(); }
	};
}

//----------------------------------------------------------------------------
#endif
