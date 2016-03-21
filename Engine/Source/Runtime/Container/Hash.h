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
#ifndef __YumeCHash_h__
#define __YumeCHash_h__
//----------------------------------------------------------------------------
#pragma once

#include <cstddef>

namespace YumeEngine
{

/// Pointer hash function.
template <class T> unsigned MakeHash(T* value)
{
    return (unsigned)((size_t)value / sizeof(T));
}

/// Const pointer hash function.
template <class T> unsigned MakeHash(const T* value)
{
    return (unsigned)((size_t)value / sizeof(T));
}

/// Generic hash function.
template <class T> unsigned MakeHash(const T& value)
{
    return value.ToHash();
}

/// Void pointer hash function.
template <> inline unsigned MakeHash(void* value)
{
    return (unsigned)(size_t)value;
}

/// Const void pointer hash function.
template <> inline unsigned MakeHash(const void* value)
{
    return (unsigned)(size_t)value;
}

/// Long long hash function.
template <> inline unsigned MakeHash(const long long& value)
{
    return (unsigned)((value >> 32) | (value & 0xffffffff));
}

/// Unsigned long long hash function.
template <> inline unsigned MakeHash(const unsigned long long& value)
{
    return (unsigned)((value >> 32) | (value & 0xffffffff));
}

/// Int hash function.
template <> inline unsigned MakeHash(const int& value)
{
    return (unsigned)value;
}

/// Unsigned hash function.
template <> inline unsigned MakeHash(const unsigned& value)
{
    return value;
}

/// Short hash function.
template <> inline unsigned MakeHash(const short& value)
{
    return (unsigned)value;
}

/// Unsigned short hash function.
template <> inline unsigned MakeHash(const unsigned short& value)
{
    return value;
}

/// Char hash function.
template <> inline unsigned MakeHash(const char& value)
{
    return (unsigned)value;
}

/// Unsigned char hash function.
template <> inline unsigned MakeHash(const unsigned char& value)
{
    return value;
}

}

#endif