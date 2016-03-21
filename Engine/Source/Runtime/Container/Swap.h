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
#ifndef __YumeSwap_h__
#define __YumeSwap_h__
//----------------------------------------------------------------------------
#pragma once

namespace YumeEngine
{

class HashBase;
class ListBase;
class String;
class VectorBase;

/// Swap two values.
template <class T> inline void Swap(T& first, T& second)
{
    T temp = first;
    first = second;
    second = temp;
}

template <> YumeAPIExport void Swap<String>(String& first, String& second);
template <> YumeAPIExport void Swap<VectorBase>(VectorBase& first, VectorBase& second);
template <> YumeAPIExport void Swap<ListBase>(ListBase& first, ListBase& second);
template <> YumeAPIExport void Swap<HashBase>(HashBase& first, HashBase& second);

}
#endif