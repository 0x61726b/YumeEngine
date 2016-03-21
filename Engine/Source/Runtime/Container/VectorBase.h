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
#ifndef __YumeVectorBase_h__
#define __YumeVectorBase_h__
//----------------------------------------------------------------------------
#pragma once
#include "YumeRequired.h"
#include "Swap.h"

namespace YumeEngine
{

/// Random access iterator.
template <class T> struct RandomAccessIterator
{
    /// Construct.
    RandomAccessIterator() :
        ptr_(0)
    {
    }

    /// Construct with an object pointer.
    explicit RandomAccessIterator(T* ptr) :
        ptr_(ptr)
    {
    }

    /// Point to the object.
    T* operator ->() const { return ptr_; }

    /// Dereference the object.
    T& operator *() const { return *ptr_; }

    /// Preincrement the pointer.
    RandomAccessIterator<T>& operator ++()
    {
        ++ptr_;
        return *this;
    }

    /// Postincrement the pointer.
    RandomAccessIterator<T> operator ++(int)
    {
        RandomAccessIterator<T> it = *this;
        ++ptr_;
        return it;
    }

    /// Predecrement the pointer.
    RandomAccessIterator<T>& operator --()
    {
        --ptr_;
        return *this;
    }

    /// Postdecrement the pointer.
    RandomAccessIterator<T> operator --(int)
    {
        RandomAccessIterator<T> it = *this;
        --ptr_;
        return it;
    }

    /// Add an offset to the pointer.
    RandomAccessIterator<T>& operator +=(int value)
    {
        ptr_ += value;
        return *this;
    }

    /// Subtract an offset from the pointer.
    RandomAccessIterator<T>& operator -=(int value)
    {
        ptr_ -= value;
        return *this;
    }

    /// Add an offset to the pointer.
    RandomAccessIterator<T> operator +(int value) const { return RandomAccessIterator<T>(ptr_ + value); }

    /// Subtract an offset from the pointer.
    RandomAccessIterator<T> operator -(int value) const { return RandomAccessIterator<T>(ptr_ - value); }

    /// Calculate offset to another iterator.
    int operator -(const RandomAccessIterator& rhs) const { return (int)(ptr_ - rhs.ptr_); }

    /// Test for equality with another iterator.
    bool operator ==(const RandomAccessIterator& rhs) const { return ptr_ == rhs.ptr_; }

    /// Test for inequality with another iterator.
    bool operator !=(const RandomAccessIterator& rhs) const { return ptr_ != rhs.ptr_; }

    /// Test for less than with another iterator.
    bool operator <(const RandomAccessIterator& rhs) const { return ptr_ < rhs.ptr_; }

    /// Test for greater than with another iterator.
    bool operator >(const RandomAccessIterator& rhs) const { return ptr_ > rhs.ptr_; }

    /// Test for less than or equal with another iterator.
    bool operator <=(const RandomAccessIterator& rhs) const { return ptr_ <= rhs.ptr_; }

    /// Test for greater than or equal with another iterator.
    bool operator >=(const RandomAccessIterator& rhs) const { return ptr_ >= rhs.ptr_; }

    /// Pointer.
    T* ptr_;
};

/// Random access const iterator.
template <class T> struct RandomAccessConstIterator
{
    /// Construct.
    RandomAccessConstIterator() :
        ptr_(0)
    {
    }

    /// Construct with an object pointer.
    explicit RandomAccessConstIterator(T* ptr) :
        ptr_(ptr)
    {
    }

    /// Construct from a non-const iterator.
    RandomAccessConstIterator(const RandomAccessIterator<T>& rhs) :
        ptr_(rhs.ptr_)
    {
    }

    /// Assign from a non-const iterator.
    RandomAccessConstIterator<T>& operator =(const RandomAccessIterator<T>& rhs)
    {
        ptr_ = rhs.ptr_;
        return *this;
    }

    /// Point to the object.
    const T* operator ->() const { return ptr_; }

    /// Dereference the object.
    const T& operator *() const { return *ptr_; }

    /// Preincrement the pointer.
    RandomAccessConstIterator<T>& operator ++()
    {
        ++ptr_;
        return *this;
    }

    /// Postincrement the pointer.
    RandomAccessConstIterator<T> operator ++(int)
    {
        RandomAccessConstIterator<T> it = *this;
        ++ptr_;
        return it;
    }

    /// Predecrement the pointer.
    RandomAccessConstIterator<T>& operator --()
    {
        --ptr_;
        return *this;
    }

    /// Postdecrement the pointer.
    RandomAccessConstIterator<T> operator --(int)
    {
        RandomAccessConstIterator<T> it = *this;
        --ptr_;
        return it;
    }

    /// Add an offset to the pointer.
    RandomAccessConstIterator<T>& operator +=(int value)
    {
        ptr_ += value;
        return *this;
    }

    /// Subtract an offset from the pointer.
    RandomAccessConstIterator<T>& operator -=(int value)
    {
        ptr_ -= value;
        return *this;
    }

    /// Add an offset to the pointer.
    RandomAccessConstIterator<T> operator +(int value) const { return RandomAccessConstIterator<T>(ptr_ + value); }

    /// Subtract an offset from the pointer.
    RandomAccessConstIterator<T> operator -(int value) const { return RandomAccessConstIterator<T>(ptr_ - value); }

    /// Calculate offset to another iterator.
    int operator -(const RandomAccessConstIterator& rhs) const { return (int)(ptr_ - rhs.ptr_); }

    /// Test for equality with another iterator.
    bool operator ==(const RandomAccessConstIterator& rhs) const { return ptr_ == rhs.ptr_; }

    /// Test for inequality with another iterator.
    bool operator !=(const RandomAccessConstIterator& rhs) const { return ptr_ != rhs.ptr_; }

    /// Test for less than with another iterator.
    bool operator <(const RandomAccessConstIterator& rhs) const { return ptr_ < rhs.ptr_; }

    /// Test for greater than with another iterator.
    bool operator >(const RandomAccessConstIterator& rhs) const { return ptr_ > rhs.ptr_; }

    /// Test for less than or equal with another iterator.
    bool operator <=(const RandomAccessConstIterator& rhs) const { return ptr_ <= rhs.ptr_; }

    /// Test for greater than or equal with another iterator.
    bool operator >=(const RandomAccessConstIterator& rhs) const { return ptr_ >= rhs.ptr_; }

    /// Pointer.
    T* ptr_;
};

/// %Vector base class.
/** Note that to prevent extra memory use due to vtable pointer, %VectorBase intentionally does not declare a virtual destructor
    and therefore %VectorBase pointers should never be used.
  */
class YumeAPIExport VectorBase
{
public:
    /// Construct.
    VectorBase() :
        size_(0),
        capacity_(0),
        buffer_(0)
    {
    }

    /// Swap with another vector.
    void Swap(VectorBase& rhs)
    {
        YumeEngine::Swap(size_, rhs.size_);
        YumeEngine::Swap(capacity_, rhs.capacity_);
        YumeEngine::Swap(buffer_, rhs.buffer_);
    }

protected:
    static unsigned char* AllocateBuffer(unsigned size);

    /// Size of vector.
    unsigned size_;
    /// Buffer capacity.
    unsigned capacity_;
    /// Buffer.
    unsigned char* buffer_;
};

}
#endif