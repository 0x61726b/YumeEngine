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
#ifndef YumeHashSet_h__
#define YumeHashSet_h__
//----------------------------------------------------------------------------

#pragma once

#include "HashBase.h"
#include "Core/YumeSortAlgorithms.h"

#include <cassert>

namespace YumeEngine
{

/// Hash set template class.
template <class T> class HashSet : public HashBase
{
public:
    /// Hash set node.
    struct Node : public HashNodeBase
    {
        /// Construct undefined.
        Node()
        {
        }

        /// Construct with key.
        Node(const T& key) :
            key_(key)
        {
        }

        /// Key.
        T key_;

        /// Return next node.
        Node* Next() const { return static_cast<Node*>(next_); }

        /// Return previous node.
        Node* Prev() const { return static_cast<Node*>(prev_); }

        /// Return next node in the bucket.
        Node* Down() const { return static_cast<Node*>(down_); }
    };

    /// Hash set node iterator.
    struct Iterator : public HashIteratorBase
    {
        /// Construct.
        Iterator()
        {
        }

        /// Construct with a node pointer.
        Iterator(Node* ptr) :
            HashIteratorBase(ptr)
        {
        }

        /// Preincrement the pointer.
        Iterator& operator ++()
        {
            GotoNext();
            return *this;
        }

        /// Postincrement the pointer.
        Iterator operator ++(int)
        {
            Iterator it = *this;
            GotoNext();
            return it;
        }

        /// Predecrement the pointer.
        Iterator& operator --()
        {
            GotoPrev();
            return *this;
        }

        /// Postdecrement the pointer.
        Iterator operator --(int)
        {
            Iterator it = *this;
            GotoPrev();
            return it;
        }

        /// Point to the key.
        const T* operator ->() const { return &(static_cast<Node*>(ptr_))->key_; }

        /// Dereference the key.
        const T& operator *() const { return (static_cast<Node*>(ptr_))->key_; }
    };

    /// Hash set node const iterator.
    struct ConstIterator : public HashIteratorBase
    {
        /// Construct.
        ConstIterator()
        {
        }

        /// Construct with a node pointer.
        ConstIterator(Node* ptr) :
            HashIteratorBase(ptr)
        {
        }

        /// Construct from a non-const iterator.
        ConstIterator(const Iterator& rhs) :
            HashIteratorBase(rhs.ptr_)
        {
        }

        /// Assign from a non-const iterator.
        ConstIterator& operator =(const Iterator& rhs)
        {
            ptr_ = rhs.ptr_;
            return *this;
        }

        /// Preincrement the pointer.
        ConstIterator& operator ++()
        {
            GotoNext();
            return *this;
        }

        /// Postincrement the pointer.
        ConstIterator operator ++(int)
        {
            ConstIterator it = *this;
            GotoNext();
            return it;
        }

        /// Predecrement the pointer.
        ConstIterator& operator --()
        {
            GotoPrev();
            return *this;
        }

        /// Postdecrement the pointer.
        ConstIterator operator --(int)
        {
            ConstIterator it = *this;
            GotoPrev();
            return it;
        }

        /// Point to the key.
        const T* operator ->() const { return &(static_cast<Node*>(ptr_))->key_; }

        /// Dereference the key.
        const T& operator *() const { return (static_cast<Node*>(ptr_))->key_; }
    };

    /// Construct empty.
    HashSet()
    {
        // Reserve the tail node
        allocator_ = AllocatorInitialize((unsigned)sizeof(Node));
        head_ = tail_ = ReserveNode();
    }

    /// Construct from another hash set.
    HashSet(const HashSet<T>& set)
    {
        // Reserve the tail node + initial capacity according to the set's size
        allocator_ = AllocatorInitialize((unsigned)sizeof(Node), set.size() + 1);
        head_ = tail_ = ReserveNode();
        *this = set;
    }

    /// Destruct.
    ~HashSet()
    {
        clear();
        FreeNode(Tail());
        AllocatorUninitialize(allocator_);
        delete[] ptrs_;
    }

    /// Assign a hash set.
    HashSet& operator =(const HashSet<T>& rhs)
    {
        clear();
        insert(rhs);
        return *this;
    }

    /// Add-assign a value.
    HashSet& operator +=(const T& rhs)
    {
        insert(rhs);
        return *this;
    }

    /// Add-assign a hash set.
    HashSet& operator +=(const HashSet<T>& rhs)
    {
        insert(rhs);
        return *this;
    }

    /// Test for equality with another hash set.
    bool operator ==(const HashSet<T>& rhs) const
    {
        if (rhs.size() != size())
            return false;

        ConstIterator it = begin();
        while (it != end())
        {
            if (!rhs.Contains(*it))
                return false;
            ++it;
        }

        return true;
    }

    /// Test for inequality with another hash set.
    bool operator !=(const HashSet<T>& rhs) const
    {
        if (rhs.size() != size())
            return true;

        ConstIterator it = begin();
        while (it != end())
        {
            if (!rhs.Contains(*it))
                return true;
            ++it;
        }

        return false;
    }

    /// Insert a key. Return an iterator to it.
    Iterator insert(const T& key)
    {
        // If no pointers yet, allocate with minimum bucket count
        if (!ptrs_)
        {
            AllocateBuckets(size(), MIN_BUCKETS);
            Rehash();
        }

        unsigned hashKey = Hash(key);

        Node* existing = FindNode(key, hashKey);
        if (existing)
            return Iterator(existing);

        Node* newNode = InsertNode(Tail(), key);
        newNode->down_ = Ptrs()[hashKey];
        Ptrs()[hashKey] = newNode;

        // Rehash if the maximum load factor has been exceeded
        if (size() > NumBuckets() * MAX_LOAD_FACTOR)
        {
            AllocateBuckets(size(), NumBuckets() << 1);
            Rehash();
        }

        return Iterator(newNode);
    }

    /// Insert a set.
    void insert(const HashSet<T>& set)
    {
        ConstIterator it = set.begin();
        ConstIterator end = set.end();
        while (it != end)
            insert(*it++);
    }

    /// Insert a key by iterator. Return iterator to the value.
    Iterator insert(const ConstIterator& it)
    {
        return Iterator(InsertNode(*it));
    }

    /// Erase a key. Return true if was found.
    bool erase(const T& key)
    {
        if (!ptrs_)
            return false;

        unsigned hashKey = Hash(key);

        Node* previous;
        Node* node = FindNode(key, hashKey, previous);
        if (!node)
            return false;

        if (previous)
            previous->down_ = node->down_;
        else
            Ptrs()[hashKey] = node->down_;

        EraseNode(node);
        return true;
    }

    /// Erase a key by iterator. Return iterator to the next key.
    Iterator erase(const Iterator& it)
    {
        if (!ptrs_ || !it.ptr_)
            return end();

        Node* node = static_cast<Node*>(it.ptr_);
        Node* next = node->Next();

        unsigned hashKey = Hash(node->key_);

        Node* previous = 0;
        Node* current = static_cast<Node*>(Ptrs()[hashKey]);
        while (current && current != node)
        {
            previous = current;
            current = current->Down();
        }

        assert(current == node);

        if (previous)
            previous->down_ = node->down_;
        else
            Ptrs()[hashKey] = node->down_;

        EraseNode(node);
        return Iterator(next);
    }

    /// Clear the set.
    void clear()
    {
        // Prevent find() from returning anything while the map is being cleared
        ResetPtrs();

        if (size())
        {
            for (Iterator i = begin(); i != end();)
            {
                FreeNode(static_cast<Node*>(i++.ptr_));
                i.ptr_->prev_ = 0;
            }

            head_ = tail_;
            Setsize(0);
        }
    }

    /// Sort keys. After sorting the set can be iterated in order until new elements are inserted.
    void Sort()
    {
        unsigned numKeys = size();
        if (!numKeys)
            return;

        Node** ptrs = new Node* [numKeys];
        Node* ptr = Head();

        for (unsigned i = 0; i < numKeys; ++i)
        {
            ptrs[i] = ptr;
            ptr = ptr->Next();
        }

        YumeEngine::Sort(RandomAccessIterator<Node*>(ptrs), RandomAccessIterator<Node*>(ptrs + numKeys), CompareNodes);

        head_ = ptrs[0];
        ptrs[0]->prev_ = 0;
        for (unsigned i = 1; i < numKeys; ++i)
        {
            ptrs[i - 1]->next_ = ptrs[i];
            ptrs[i]->prev_ = ptrs[i - 1];
        }
        ptrs[numKeys - 1]->next_ = tail_;
        tail_->prev_ = ptrs[numKeys - 1];

        delete[] ptrs;
    }

    /// Rehash to a specific bucket count, which must be a power of two. Return true if successful.
    bool Rehash(unsigned numBuckets)
    {
        if (numBuckets == NumBuckets())
            return true;
        if (!numBuckets || numBuckets < size() / MAX_LOAD_FACTOR)
            return false;

        // Check for being power of two
        unsigned check = numBuckets;
        while (!(check & 1))
            check >>= 1;
        if (check != 1)
            return false;

        AllocateBuckets(size(), numBuckets);
        Rehash();
        return true;
    }

    /// Return iterator to the key, or end iterator if not found.
    Iterator find(const T& key)
    {
        if (!ptrs_)
            return end();

        unsigned hashKey = Hash(key);
        Node* node = FindNode(key, hashKey);
        if (node)
            return Iterator(node);
        else
            return end();
    }

    /// Return const iterator to the key, or end iterator if not found.
    ConstIterator find(const T& key) const
    {
        if (!ptrs_)
            return end();

        unsigned hashKey = Hash(key);
        Node* node = FindNode(key, hashKey);
        if (node)
            return ConstIterator(node);
        else
            return end();
    }

    /// Return whether contains a key.
    bool Contains(const T& key) const
    {
        if (!ptrs_)
            return false;

        unsigned hashKey = Hash(key);
        return FindNode(key, hashKey) != 0;
    }

    /// Return iterator to the beginning.
    Iterator begin() { return Iterator(Head()); }

    /// Return iterator to the beginning.
    ConstIterator begin() const { return ConstIterator(Head()); }

    /// Return iterator to the end.
    Iterator end() { return Iterator(Tail()); }

    /// Return iterator to the end.
    ConstIterator end() const { return ConstIterator(Tail()); }

    /// Return first key.
    const T& front() const { return *begin(); }

    /// Return last key.
    const T& back() const { return *(--end()); }

private:
    /// Return the head node.
    Node* Head() const { return static_cast<Node*>(head_); }

    /// Return the tail node.
    Node* Tail() const { return static_cast<Node*>(tail_); }

    /// Find a node from the buckets. Do not call if the buckets have not been allocated.
    Node* FindNode(const T& key, unsigned hashKey) const
    {
        Node* node = static_cast<Node*>(Ptrs()[hashKey]);
        while (node)
        {
            if (node->key_ == key)
                return node;
            node = node->Down();
        }

        return 0;
    }

    /// Find a node and the previous node from the buckets. Do not call if the buckets have not been allocated.
    Node* FindNode(const T& key, unsigned hashKey, Node*& previous) const
    {
        previous = 0;

        Node* node = static_cast<Node*>(Ptrs()[hashKey]);
        while (node)
        {
            if (node->key_ == key)
                return node;
            previous = node;
            node = node->Down();
        }

        return 0;
    }

    /// Insert a node into the list. Return the new node.
    Node* InsertNode(Node* dest, const T& key)
    {
        if (!dest)
            return 0;

        Node* newNode = ReserveNode(key);
        Node* prev = dest->Prev();
        newNode->next_ = dest;
        newNode->prev_ = prev;
        if (prev)
            prev->next_ = newNode;
        dest->prev_ = newNode;

        // Reassign the head node if necessary
        if (dest == Head())
            head_ = newNode;

        Setsize(size() + 1);

        return newNode;
    }

    /// Erase a node from the list. Return pointer to the next element, or to the end if could not erase.
    Node* EraseNode(Node* node)
    {
        // The tail node can not be removed
        if (!node || node == tail_)
            return Tail();

        Node* prev = node->Prev();
        Node* next = node->Next();
        if (prev)
            prev->next_ = next;
        next->prev_ = prev;

        // Reassign the head node if necessary
        if (node == Head())
            head_ = next;

        FreeNode(node);
        Setsize(size() - 1);

        return next;
    }

    /// Reserve a node.
    Node* ReserveNode()
    {
        Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
        new(newNode) Node();
        return newNode;
    }

    /// Reserve a node with specified key.
    Node* ReserveNode(const T& key)
    {
        Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
        new(newNode) Node(key);
        return newNode;
    }

    /// Free a node.
    void FreeNode(Node* node)
    {
        (node)->~Node();
        AllocatorFree(allocator_, node);
    }

    /// Rehash the buckets.
    void Rehash()
    {
        for (Iterator it = begin(); it != end(); ++it)
        {
            Node* node = static_cast<Node*>(it.ptr_);
            unsigned hashKey = Hash(*it);
            node->down_ = Ptrs()[hashKey];
            Ptrs()[hashKey] = node;
        }
    }

    /// Compare two nodes.
    static bool CompareNodes(Node*& lhs, Node*& rhs) { return lhs->key_ < rhs->key_; }

    /// Compute a hash based on the key and the bucket size
    unsigned Hash(const T& key) const { return MakeHash(key) & (NumBuckets() - 1); }
};

}

namespace std
{

template <class T> typename YumeEngine::HashSet<T>::const_iterator begin(const YumeEngine::HashSet<T>& v) { return v.begin(); }

template <class T> typename YumeEngine::HashSet<T>::const_iterator end(const YumeEngine::HashSet<T>& v) { return v.end(); }

template <class T> typename YumeEngine::HashSet<T>::Iterator begin(YumeEngine::HashSet<T>& v) { return v.begin(); }

template <class T> typename YumeEngine::HashSet<T>::Iterator end(YumeEngine::HashSet<T>& v) { return v.end(); }

}

#endif