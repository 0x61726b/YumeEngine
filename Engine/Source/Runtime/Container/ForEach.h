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
#ifndef __YumeForEach_h__
#define __YumeForEach_h__
//----------------------------------------------------------------------------
#pragma once

#include "Vector.h"
#include "List.h"
#include "HashSet.h"
#include "HashMap.h"

#include <algorithm>

// VS2010+ and other compilers: use std::begin(), std::end() & range based for
// C++11 features need to be enabled
#if !defined(_MSC_VER) || _MSC_VER > 1600
#define foreach(VAL, VALS) for (VAL : VALS)
// Fallback solution for VS2010. Will have problem with break statement.
// See https://github.com/urho3d/Urho3D/issues/561
#else

namespace YumeEngine
{

template<typename T>
struct false_wrapper {
    false_wrapper(const T& value) : value(value) { }
    operator bool() const { return false; }
    T value;
};

template<typename T>
false_wrapper<T> make_false_wrapper(const T& value) {
    return false_wrapper<T>(value);
}

// vector support functions

template <class T>
YumeEngine::RandomAccessIterator<T> Begin(YumeEngine::Vector<T> &v) {
    return v.Begin();
}
template <class T>
YumeEngine::RandomAccessIterator<T> Begin(YumeEngine::Vector<T> *v) {
    return v->Begin();
}

template <class T>
YumeEngine::RandomAccessConstIterator<T> Begin(const YumeEngine::Vector<T> &v) {
    return v.Begin();
}
template <class T>
YumeEngine::RandomAccessConstIterator<T> Begin(const YumeEngine::Vector<T> *v) {
    return v->Begin();
}

template <class T>
YumeEngine::RandomAccessIterator<T> End(YumeEngine::Vector<T> &v) {
    return v.End();
}
template <class T>
YumeEngine::RandomAccessIterator<T> End(YumeEngine::Vector<T> *v) {
    return v->End();
}

template <class T>
YumeEngine::RandomAccessConstIterator<T> End(const YumeEngine::Vector<T> &v) {
    return v.End();
}
template <class T>
YumeEngine::RandomAccessConstIterator<T> End(const YumeEngine::Vector<T> *v) {
    return v->End();
}

// podvector support functions

template <class T>
YumeEngine::RandomAccessIterator<T> Begin(YumeEngine::PODVector<T> &v) {
    return v.Begin();
}
template <class T>
YumeEngine::RandomAccessIterator<T> Begin(YumeEngine::PODVector<T> *v) {
    return v->Begin();
}

template <class T>
YumeEngine::RandomAccessConstIterator<T> Begin(const YumeEngine::PODVector<T> &v) {
    return v.Begin();
}
template <class T>
YumeEngine::RandomAccessConstIterator<T> Begin(const YumeEngine::PODVector<T> *v) {
    return v->Begin();
}

template <class T>
YumeEngine::RandomAccessIterator<T> End(YumeEngine::PODVector<T> &v) {
    return v.End();
}
template <class T>
YumeEngine::RandomAccessIterator<T> End(YumeEngine::PODVector<T> *v) {
    return v->End();
}

template <class T>
YumeEngine::RandomAccessConstIterator<T> End(const YumeEngine::PODVector<T> &v) {
    return v.End();
}
template <class T>
YumeEngine::RandomAccessConstIterator<T> End(const YumeEngine::PODVector<T> *v) {
    return v->End();
}

}

#define foreach(VAL, VALS) \
    if (const auto& _foreach_begin = YumeEngine::make_false_wrapper(YumeEngine::Begin(VALS))) { } else \
    if (const auto& _foreach_end = YumeEngine::make_false_wrapper(YumeEngine::End(VALS))) { } else \
    for (auto it = _foreach_begin.value; it != _foreach_end.value; ++it) \
    if (bool _foreach_flag = false) { } else \
    for (VAL = *it; !_foreach_flag; _foreach_flag = true)

#endif

#endif