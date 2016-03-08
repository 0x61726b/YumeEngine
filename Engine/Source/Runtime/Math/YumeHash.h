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

	/// 32-bit hash value for a string.
	class YumeAPIExport YumeHash
	{
	public:
		/// Construct with zero value.
		YumeHash():
			value_(0)
		{
		}

		/// Copy-construct from another hash.
		YumeHash(const YumeHash& rhs):
			value_(rhs.value_)
		{
		}

		/// Construct with an initial value.
		explicit YumeHash(unsigned value):
			value_(value)
		{
		}

		/// Construct from a C string case-insensitively.
		YumeHash(const char* str);
		/// Construct from a string case-insensitively.
		YumeHash(const YumeString& str);

		/// Assign from another hash.
		YumeHash& operator =(const YumeHash& rhs)
		{
			value_ = rhs.value_;
			return *this;
		}

		/// Add a hash.
		YumeHash operator +(const YumeHash& rhs) const
		{
			YumeHash ret;
			ret.value_ = value_ + rhs.value_;
			return ret;
		}

		/// Add-assign a hash.
		YumeHash& operator +=(const YumeHash& rhs)
		{
			value_ += rhs.value_;
			return *this;
		}

		// Test for equality with another hash.
		bool operator ==(const YumeHash& rhs) const { return value_ == rhs.value_; }

		/// Test for inequality with another hash.
		bool operator !=(const YumeHash& rhs) const { return value_ != rhs.value_; }

		/// Test if less than another hash.
		bool operator <(const YumeHash& rhs) const { return value_ < rhs.value_; }

		/// Test if greater than another hash.
		bool operator >(const YumeHash& rhs) const { return value_ > rhs.value_; }

		/// Return true if nonzero hash value.
		operator bool() const { return value_ != 0; }

		/// Return hash value.
		unsigned Value() const { return value_; }

		/// Return as string.
		YumeString ToString() const;

		/// Return hash value for HashSet & HashMap.
		unsigned ToHash() const { return value_; }

		/// Calculate hash value case-insensitively from a C string.
		static unsigned Calculate(const char* str);

		/// Zero hash.
		static const YumeHash ZERO;

	private:
		/// Hash value.
		unsigned value_;
	};
}


//----------------------------------------------------------------------------
#endif
