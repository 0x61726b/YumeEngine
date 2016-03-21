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
#ifndef __YumePair_h__
#define __YumePair_h__
//----------------------------------------------------------------------------
#pragma once

#include "Hash.h"

namespace YumeEngine
{

/// %Pair template class.
template <class T, class U> class Pair
{
public:
    /// Construct undefined.
    Pair()
    {
    }

    /// Construct with values.
    Pair(const T& first, const U& second) :
        first(first),
        second(second)
    {
    }

    /// Test for equality with another pair.
    bool operator ==(const Pair<T, U>& rhs) const { return first == rhs.first && second == rhs.second; }

    /// Test for inequality with another pair.
    bool operator !=(const Pair<T, U>& rhs) const { return first != rhs.first || second != rhs.second; }

    /// Test for less than with another pair.
    bool operator <(const Pair<T, U>& rhs) const
    {
        if (first < rhs.first)
            return true;
        if (first != rhs.first)
            return false;
        return second < rhs.second;
    }

    /// Test for greater than with another pair.
    bool operator >(const Pair<T, U>& rhs) const
    {
        if (first > rhs.first)
            return true;
        if (first != rhs.first)
            return false;
        return second > rhs.second;
    }

    /// Return hash value for HashSet & HashMap.
    unsigned ToHash() const { return (MakeHash(first) & 0xffff) | (MakeHash(second) << 16); }

    /// First value.
    T first;
    /// Second value.
    U second;
};

/// Construct a pair.
template <class T, class U> Pair<T, U> MakePair(const T& first, const U& second)
{
    return Pair<T, U>(first, second);
}

}
#endif