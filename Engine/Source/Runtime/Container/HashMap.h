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
#ifndef __YumeHashMap_h__
#define __YumeHashMap_h__
//----------------------------------------------------------------------------

#pragma once

#include "HashBase.h"
#include "Pair.h"
#include "Core/YumeSortAlgorithms.h"
#include "Vector.h"

#include <cassert>

namespace YumeEngine
{

/// Hash map template class.
template <class T, class U> class HashMap : public HashBase
{
public:
    typedef T KeyType;
    typedef U ValueType;

    /// Hash map key-value pair with const key.
    class KeyValue
    {
    public:
        /// Construct with default key.
        KeyValue() :
            first(T())
        {
        }

        /// Construct with key and value.
        KeyValue(const T& first, const U& second) :
            first(first),
            second(second)
        {
        }

        /// Copy-construct.
        KeyValue(const KeyValue& value) :
            first(value.first),
            second(value.second)
        {
        }

        /// Test for equality with another pair.
        bool operator ==(const KeyValue& rhs) const { return first == rhs.first && second == rhs.second; }

        /// Test for inequality with another pair.
        bool operator !=(const KeyValue& rhs) const { return first != rhs.first || second != rhs.second; }

        /// Key.
        const T first;
        /// Value.
        U second;

    private:
        /// Prevent assignment.
        KeyValue& operator =(const KeyValue& rhs);
    };

    /// Hash map node.
    struct Node : public HashNodeBase
    {
        /// Construct undefined.
        Node()
        {
        }

        /// Construct with key and value.
        Node(const T& key, const U& value) :
            pair_(key, value)
        {
        }

        /// Key-value pair.
        KeyValue pair_;

        /// Return next node.
        Node* Next() const { return static_cast<Node*>(next_); }

        /// Return previous node.
        Node* Prev() const { return static_cast<Node*>(prev_); }

        /// Return next node in the bucket.
        Node* Down() const { return static_cast<Node*>(down_); }
    };

    /// Hash map node iterator.
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

        /// Point to the pair.
        KeyValue* operator ->() const { return &(static_cast<Node*>(ptr_))->pair_; }

        /// Dereference the pair.
        KeyValue& operator *() const { return (static_cast<Node*>(ptr_))->pair_; }
    };

    /// Hash map node const iterator.
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

        /// Point to the pair.
        const KeyValue* operator ->() const { return &(static_cast<Node*>(ptr_))->pair_; }

        /// Dereference the pair.
        const KeyValue& operator *() const { return (static_cast<Node*>(ptr_))->pair_; }
    };

    /// Construct empty.
    HashMap()
    {
        // Reserve the tail node
        allocator_ = AllocatorInitialize((unsigned)sizeof(Node));
        head_ = tail_ = ReserveNode();
    }

    /// Construct from another hash map.
    HashMap(const HashMap<T, U>& map)
    {
        // Reserve the tail node + initial capacity according to the map's size
        allocator_ = AllocatorInitialize((unsigned)sizeof(Node), map.size() + 1);
        head_ = tail_ = ReserveNode();
        *this = map;
    }

    /// Destruct.
    ~HashMap()
    {
        clear();
        FreeNode(Tail());
        AllocatorUninitialize(allocator_);
        delete[] ptrs_;
    }

    /// Assign a hash map.
    HashMap& operator =(const HashMap<T, U>& rhs)
    {
        clear();
        insert(rhs);
        return *this;
    }

    /// Add-assign a pair.
    HashMap& operator +=(const Pair<T, U>& rhs)
    {
        insert(rhs);
        return *this;
    }

    /// Add-assign a hash map.
    HashMap& operator +=(const HashMap<T, U>& rhs)
    {
        insert(rhs);
        return *this;
    }

    /// Test for equality with another hash map.
    bool operator ==(const HashMap<T, U>& rhs) const
    {
        if (rhs.size() != size())
            return false;

        ConstIterator i = begin();
        while (i != end())
        {
            ConstIterator j = rhs.find(i->first);
            if (j == rhs.end() || j->second != i->second)
                return false;
            ++i;
        }

        return true;
    }

    /// Test for inequality with another hash map.
    bool operator !=(const HashMap<T, U>& rhs) const
    {
        if (rhs.size() != size())
            return true;

        ConstIterator i = begin();
        while (i != end())
        {
            ConstIterator j = rhs.find(i->first);
            if (j == rhs.end() || j->second != i->second)
                return true;
            ++i;
        }

        return false;
    }

    /// Index the map. Create a new pair if key not found.
    U& operator [](const T& key)
    {
        if (!ptrs_)
            return InsertNode(key, U(), false)->pair_.second;

        unsigned hashKey = Hash(key);

        Node* node = FindNode(key, hashKey);
        return node ? node->pair_.second : InsertNode(key, U(), false)->pair_.second;
    }

    /// Index the map. Return null if key is not found, does not create a new pair.
    U* operator [](const T& key) const
    {
        if (!ptrs_)
            return 0;

        unsigned hashKey = Hash(key);

        Node* node = FindNode(key, hashKey);
        return node ? &node->pair_.second : 0;
    }

    /// Insert a pair. Return an iterator to it.
    Iterator insert(const Pair<T, U>& pair)
    {
        return Iterator(InsertNode(pair.first, pair.second));
    }

    /// Insert a map.
    void insert(const HashMap<T, U>& map)
    {
        ConstIterator it = map.begin();
        ConstIterator end = map.end();
        while (it != end)
        {
            InsertNode(it->first, it->second);
            ++it;
        }
    }

    /// Insert a pair by iterator. Return iterator to the value.
    Iterator insert(const ConstIterator& it) { return Iterator(InsertNode(it->first, it->second)); }

    /// Insert a range by iterators.
    void insert(const ConstIterator& start, const ConstIterator& end)
    {
        ConstIterator it = start;
        while (it != end)
            InsertNode(*it++);
    }

    /// Erase a pair by key. Return true if was found.
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

    /// Erase a pair by iterator. Return iterator to the next pair.
    Iterator erase(const Iterator& it)
    {
        if (!ptrs_ || !it.ptr_)
            return end();

        Node* node = static_cast<Node*>(it.ptr_);
        Node* next = node->Next();

        unsigned hashKey = Hash(node->pair_.first);

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

    /// Clear the map.
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

    /// Sort pairs. After sorting the map can be iterated in order until new elements are inserted.
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

    /// Return iterator to the pair with key, or end iterator if not found.
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

    /// Return const iterator to the pair with key, or end iterator if not found.
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

    /// Return whether contains a pair with key.
    bool Contains(const T& key) const
    {
        if (!ptrs_)
            return false;

        unsigned hashKey = Hash(key);
        return FindNode(key, hashKey) != 0;
    }

    /// Return all the keys.
    Vector<T> Keys() const
    {
        Vector<T> result;
        result.Reserve(size());
        for (ConstIterator i = begin(); i != end(); ++i)
            result.Push(i->first);
        return result;
    }

    /// Return all the values.
    Vector<U> Values() const
    {
        Vector<U> result;
        result.Reserve(size());
        for (ConstIterator i = begin(); i != end(); ++i)
            result.Push(i->second);
        return result;
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
    const T& Front() const { return *begin(); }

    /// Return last key.
    const T& Back() const { return *(--end()); }

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
            if (node->pair_.first == key)
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
            if (node->pair_.first == key)
                return node;
            previous = node;
            node = node->Down();
        }

        return 0;
    }

    /// Insert a key and value and return either the new or existing node.
    Node* InsertNode(const T& key, const U& value, bool findExisting = true)
    {
        // If no pointers yet, allocate with minimum bucket count
        if (!ptrs_)
        {
            AllocateBuckets(size(), MIN_BUCKETS);
            Rehash();
        }

        unsigned hashKey = Hash(key);

        if (findExisting)
        {
            // If exists, just change the value
            Node* existing = FindNode(key, hashKey);
            if (existing)
            {
                existing->pair_.second = value;
                return existing;
            }
        }

        Node* newNode = InsertNode(Tail(), key, value);
        newNode->down_ = Ptrs()[hashKey];
        Ptrs()[hashKey] = newNode;

        // Rehash if the maximum load factor has been exceeded
        if (size() > NumBuckets() * MAX_LOAD_FACTOR)
        {
            AllocateBuckets(size(), NumBuckets() << 1);
            Rehash();
        }

        return newNode;
    }

    /// Insert a node into the list. Return the new node.
    Node* InsertNode(Node* dest, const T& key, const U& value)
    {
        if (!dest)
            return 0;

        Node* newNode = ReserveNode(key, value);
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

    /// Reserve a node with specified key and value.
    Node* ReserveNode(const T& key, const U& value)
    {
        Node* newNode = static_cast<Node*>(AllocatorReserve(allocator_));
        new(newNode) Node(key, value);
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
        for (Iterator i = begin(); i != end(); ++i)
        {
            Node* node = static_cast<Node*>(i.ptr_);
            unsigned hashKey = Hash(i->first);
            node->down_ = Ptrs()[hashKey];
            Ptrs()[hashKey] = node;
        }
    }

    /// Compare two nodes.
    static bool CompareNodes(Node*& lhs, Node*& rhs) { return lhs->pair_.first < rhs->pair_.first; }

    /// Compute a hash based on the key and the bucket size
    unsigned Hash(const T& key) const { return MakeHash(key) & (NumBuckets() - 1); }
};

}

namespace std
{

template <class T, class U> typename YumeEngine::HashMap<T, U>::ConstIterator begin(const YumeEngine::HashMap<T, U>& v)
{
    return v.begin();
}

template <class T, class U> typename YumeEngine::HashMap<T, U>::ConstIterator end(const YumeEngine::HashMap<T, U>& v) { return v.end(); }

template <class T, class U> typename YumeEngine::HashMap<T, U>::Iterator begin(YumeEngine::HashMap<T, U>& v) { return v.begin(); }

template <class T, class U> typename YumeEngine::HashMap<T, U>::Iterator end(YumeEngine::HashMap<T, U>& v) { return v.end(); }

}
#endif