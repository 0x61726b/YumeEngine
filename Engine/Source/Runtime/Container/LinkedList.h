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
#ifndef __YumeLinkedList_h__
#define __YumeLinkedList_h__
//----------------------------------------------------------------------------

#pragma once

#include "YumeRequired.h"

namespace YumeEngine
{

/// Singly-linked list node base class.
struct YumeAPIExport LinkedListNode
{
    /// Construct.
    LinkedListNode() :
        next_(0)
    {
    }

    /// Pointer to next node.
    LinkedListNode* next_;
};

/// Singly-linked list template class. Elements must inherit from LinkedListNode.
template <class T> class LinkedList
{
public:
    /// Construct empty.
    LinkedList() :
        head_(0)
    {
    }

    /// Destruct.
    ~LinkedList()
    {
        Clear();
    }

    /// Remove all elements.
    void Clear()
    {
        T* element = head_;
        while (element)
        {
            T* next = Next(element);
            delete element;
            element = next;
        }
    }

    /// Insert an element at the beginning.
    void InsertFront(T* element)
    {
        if (element)
        {
            element->next_ = head_;
            head_ = element;
        }
    }

    /// Insert an element at the end.
    void Insert(T* element)
    {
        if (head_)
        {
            T* tail = Last();
            element->next_ = tail->next_;
            tail->next_ = element;
        }
        else
        {
            element->next_ = head_;
            head_ = element;
        }
    }

    /// Erase an element. Return true if successful.
    bool Erase(T* element)
    {
        if (element && head_)
        {
            if (element == head_)
            {
                head_ = Next(element);
                delete element;
                return true;
            }
            else
            {
                T* tail = head_;
                while (tail && tail->next_ != element)
                    tail = Next(tail);
                if (tail)
                {
                    tail->next_ = element->next_;
                    delete element;
                    return true;
                }
            }
        }

        return false;
    }

    /// Erase an element when the previous element is known (optimization.) Return true if successful.
    bool Erase(T* element, T* previous)
    {
        if (previous && previous->next_ == element)
        {
            previous->next_ = element->next_;
            delete element;
            return true;
        }
        else if (!previous)
        {
            if (head_ == element)
            {
                head_ = Next(element);
                delete element;
                return true;
            }
        }

        return false;
    }

    /// Return first element, or null if empty.
    T* First() const { return head_; }

    /// Return last element, or null if empty.
    T* Last() const
    {
        T* element = head_;
        if (element)
        {
            while (element->next_)
                element = Next(element);
        }
        return element;
    }

    /// Return next element, or null if no more elements.
    T* Next(T* element) const { return element ? static_cast<T*>(element->next_) : 0; }

    /// Return whether is empty.
    bool Empty() const { return head_ == 0; }

private:
    /// First element.
    T* head_;
};

}
#endif