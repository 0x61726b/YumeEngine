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
#ifndef __YumeHashBase_h__
#define __YumeHashBase_h__
//----------------------------------------------------------------------------
#pragma once

#include "YumeRequired.h"

#include "Allocator.h"
#include "Hash.h"
#include "Swap.h"

namespace YumeEngine
{

/// Hash set/map node base class.
struct HashNodeBase
{
    /// Construct.
    HashNodeBase() :
        down_(0),
        prev_(0),
        next_(0)
    {
    }

    /// Next node in the bucket.
    HashNodeBase* down_;
    /// Previous node.
    HashNodeBase* prev_;
    /// Next node.
    HashNodeBase* next_;
};

/// Hash set/map iterator base class.
struct HashIteratorBase
{
    /// Construct.
    HashIteratorBase() :
        ptr_(0)
    {
    }

    /// Construct with a node pointer.
    explicit HashIteratorBase(HashNodeBase* ptr) :
        ptr_(ptr)
    {
    }

    /// Test for equality with another iterator.
    bool operator ==(const HashIteratorBase& rhs) const { return ptr_ == rhs.ptr_; }

    /// Test for inequality with another iterator.
    bool operator !=(const HashIteratorBase& rhs) const { return ptr_ != rhs.ptr_; }

    /// Go to the next node.
    void GotoNext()
    {
        if (ptr_)
            ptr_ = ptr_->next_;
    }

    /// Go to the previous node.
    void GotoPrev()
    {
        if (ptr_)
            ptr_ = ptr_->prev_;
    }

    /// Node pointer.
    HashNodeBase* ptr_;
};

/// Hash set/map base class.
/** Note that to prevent extra memory use due to vtable pointer, %HashBase intentionally does not declare a virtual destructor
    and therefore %HashBase pointers should never be used.
  */
class YumeAPIExport HashBase
{
public:
    /// Initial amount of buckets.
    static const unsigned MIN_BUCKETS = 8;
    /// Maximum load factor.
    static const unsigned MAX_LOAD_FACTOR = 4;

    /// Construct.
    HashBase() :
        ptrs_(0),
        allocator_(0)
    {
    }

    /// Swap with another hash set or map.
    void Swap(HashBase& rhs)
    {
        YumeEngine::Swap(head_, rhs.head_);
        YumeEngine::Swap(tail_, rhs.tail_);
        YumeEngine::Swap(ptrs_, rhs.ptrs_);
        YumeEngine::Swap(allocator_, rhs.allocator_);
    }

    /// Return number of elements.
    unsigned size() const { return ptrs_ ? (reinterpret_cast<unsigned*>(ptrs_))[0] : 0; }

    /// Return number of buckets.
    unsigned NumBuckets() const { return ptrs_ ? (reinterpret_cast<unsigned*>(ptrs_))[1] : 0; }

    /// Return whether has no elements.
    bool empty() const { return size() == 0; }

protected:
    /// Allocate bucket head pointers + room for size and bucket count variables.
    void AllocateBuckets(unsigned size, unsigned numBuckets);

    /// Reset bucket head pointers.
    void ResetPtrs();

    /// Set new size.
    void Setsize(unsigned size) { if (ptrs_) (reinterpret_cast<unsigned*>(ptrs_))[0] = size; }

    /// Return bucket head pointers.
    HashNodeBase** Ptrs() const { return ptrs_ ? ptrs_ + 2 : 0; }

    /// List head node pointer.
    HashNodeBase* head_;
    /// List tail node pointer.
    HashNodeBase* tail_;
    /// Bucket head pointers.
    HashNodeBase** ptrs_;
    /// Node allocator.
    AllocatorBlock* allocator_;
};

}
#endif