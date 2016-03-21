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
#ifndef __YumeVector_h__
#define __YumeVector_h__
//----------------------------------------------------------------------------
#pragma once

#include "VectorBase.h"

#include <cassert>
#include <cstring>
#include <new>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:6293)
#endif

namespace YumeEngine
{

/// %Vector template class.
template <class T> class Vector : public VectorBase
{
public:
    typedef T ValueType;
    typedef RandomAccessIterator<T> Iterator;
    typedef RandomAccessConstIterator<T> ConstIterator;

    /// Construct empty.
    Vector()
    {
    }

    /// Construct with initial size.
    explicit Vector(unsigned size)
    {
        resize(size, 0);
    }

    /// Construct with initial data.
    Vector(const T* data, unsigned size)
    {
        resize(size, data);
    }

    /// Construct from another vector.
    Vector(const Vector<T>& vector)
    {
        *this = vector;
    }

    /// Destruct.
    ~Vector()
    {
        clear();
        delete[] buffer_;
    }

    /// Assign from another vector.
    Vector<T>& operator =(const Vector<T>& rhs)
    {
        clear();
        resize(rhs.size_, rhs.Buffer());
        return *this;
    }

    /// Add-assign an element.
    Vector<T>& operator +=(const T& rhs)
    {
        push_back(rhs);
        return *this;
    }

    /// Add-assign another vector.
    Vector<T>& operator +=(const Vector<T>& rhs)
    {
        push_back(rhs);
        return *this;
    }

    /// Add an element.
    Vector<T> operator +(const T& rhs) const
    {
        Vector<T> ret(*this);
        ret.push_back(rhs);
        return ret;
    }

    /// Add another vector.
    Vector<T> operator +(const Vector<T>& rhs) const
    {
        Vector<T> ret(*this);
        ret.push_back(rhs);
        return ret;
    }

    /// Test for equality with another vector.
    bool operator ==(const Vector<T>& rhs) const
    {
        if (rhs.size_ != size_)
            return false;

        T* buffer = Buffer();
        T* rhsBuffer = rhs.Buffer();
        for (unsigned i = 0; i < size_; ++i)
        {
            if (buffer[i] != rhsBuffer[i])
                return false;
        }

        return true;
    }

    /// Test for inequality with another vector.
    bool operator !=(const Vector<T>& rhs) const
    {
        if (rhs.size_ != size_)
            return true;

        T* buffer = Buffer();
        T* rhsBuffer = rhs.Buffer();
        for (unsigned i = 0; i < size_; ++i)
        {
            if (buffer[i] != rhsBuffer[i])
                return true;
        }

        return false;
    }

    /// Return element at index.
    T& operator [](unsigned index)
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Return const element at index.
    const T& operator [](unsigned index) const
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Return element at index.
    T& At(unsigned index)
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Return const element at index.
    const T& At(unsigned index) const
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Add an element at the end.
#ifndef COVERITY_SCAN_MODEL
    void push_back(const T& value) { resize(size_ + 1, &value); }
#else
    // FIXME: Attempt had been made to use this model in the Coverity-Scan model file without any success
    // Probably because the model had generated a different mangled name than the one used by static analyzer
    void push_back(const T& value)
    {
        T array[] = {value};
        resize(size_ + 1, array);
    }
#endif

    /// Add another vector at the end.
    void push_back(const Vector<T>& vector) { resize(size_ + vector.size_, vector.Buffer()); }

    /// Remove the last element.
    void Pop()
    {
        if (size_)
            resize(size_ - 1, 0);
    }

    /// insert an element at position.
    void insert(unsigned pos, const T& value)
    {
        if (pos > size_)
            pos = size_;

        unsigned oldSize = size_;
        resize(size_ + 1, 0);
        MoveRange(pos + 1, pos, oldSize - pos);
        Buffer()[pos] = value;
    }

    /// insert another vector at position.
    void insert(unsigned pos, const Vector<T>& vector)
    {
        if (pos > size_)
            pos = size_;

        unsigned oldSize = size_;
        resize(size_ + vector.size_, 0);
        MoveRange(pos + vector.size_, pos, oldSize - pos);
        CopyElements(Buffer() + pos, vector.Buffer(), vector.size_);
    }

    /// insert an element by iterator.
    Iterator insert(const Iterator& dest, const T& value)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        insert(pos, value);

        return begin() + pos;
    }

    /// insert a vector by iterator.
    Iterator insert(const Iterator& dest, const Vector<T>& vector)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        insert(pos, vector);

        return begin() + pos;
    }

    /// insert a vector partially by iterators.
    Iterator insert(const Iterator& dest, const ConstIterator& start, const ConstIterator& end)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        unsigned length = (unsigned)(end - start);
        resize(size_ + length, 0);
        MoveRange(pos + length, pos, size_ - pos - length);

        T* destPtr = Buffer() + pos;
        for (ConstIterator it = start; it != end; ++it)
            *destPtr++ = *it;

        return begin() + pos;
    }

    /// insert elements.
    Iterator insert(const Iterator& dest, const T* start, const T* end)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        unsigned length = (unsigned)(end - start);
        resize(size_ + length, 0);
        MoveRange(pos + length, pos, size_ - pos - length);

        T* destPtr = Buffer() + pos;
        for (const T* i = start; i != end; ++i)
            *destPtr++ = *i;

        return begin() + pos;
    }

    /// Erase a range of elements.
    void erase(unsigned pos, unsigned length = 1)
    {
        // Return if the range is illegal
        if (pos + length > size_ || !length)
            return;

        MoveRange(pos, pos + length, size_ - pos - length);
        resize(size_ - length, 0);
    }

    /// Erase an element by iterator. Return iterator to the next element.
    Iterator erase(const Iterator& it)
    {
        unsigned pos = (unsigned)(it - begin());
        if (pos >= size_)
            return end();
        erase(pos);

        return begin() + pos;
    }

    /// Erase a range by iterators. Return iterator to the next element.
    Iterator erase(const Iterator& start, const Iterator& end)
    {
        unsigned pos = (unsigned)(start - begin());
        if (pos >= size_)
            return end();
        unsigned length = (unsigned)(end - start);
        erase(pos, length);

        return begin() + pos;
    }

    /// Erase an element if found.
    bool Remove(const T& value)
    {
        Iterator i = find(value);
        if (i != end())
        {
            erase(i);
            return true;
        }
        else
            return false;
    }

    /// Clear the vector.
    void clear() { resize(0); }

    /// Resize the vector.
    void resize(unsigned newSize) { resize(newSize, 0); }

    /// Set new capacity.
    void reserve(unsigned newCapacity)
    {
        if (newCapacity < size_)
            newCapacity = size_;

        if (newCapacity != capacity_)
        {
            T* newBuffer = 0;
            capacity_ = newCapacity;

            if (capacity_)
            {
                newBuffer = reinterpret_cast<T*>(AllocateBuffer((unsigned)(capacity_ * sizeof(T))));
                // Move the data into the new buffer
                ConstructElements(newBuffer, Buffer(), size_);
            }

            // Delete the old buffer
            DestructElements(Buffer(), size_);
            delete[] buffer_;
            buffer_ = reinterpret_cast<unsigned char*>(newBuffer);
        }
    }

    /// Reallocate so that no extra memory is used.
    void Compact() { reserve(size_); }

    /// Return iterator to value, or to the end if not found.
    Iterator find(const T& value)
    {
        Iterator it = begin();
        while (it != end() && *it != value)
            ++it;
        return it;
    }

    /// Return const iterator to value, or to the end if not found.
    ConstIterator find(const T& value) const
    {
        ConstIterator it = begin();
        while (it != end() && *it != value)
            ++it;
        return it;
    }

    /// Return whether contains a specific value.
    bool Contains(const T& value) const { return find(value) != end(); }

    /// Return iterator to the beginning.
    Iterator begin() { return Iterator(Buffer()); }

    /// Return const iterator to the beginning.
    ConstIterator begin() const { return ConstIterator(Buffer()); }

    /// Return iterator to the end.
    Iterator end() { return Iterator(Buffer() + size_); }

    /// Return const iterator to the end.
    ConstIterator end() const { return ConstIterator(Buffer() + size_); }

    /// Return first element.
    T& front()
    {
        assert(size_);
        return Buffer()[0];
    }

    /// Return const first element.
    const T& front() const
    {
        assert(size_);
        return Buffer()[0];
    }

    /// Return last element.
    T& back()
    {
        assert(size_);
        return Buffer()[size_ - 1];
    }

    /// Return const last element.
    const T& back() const
    {
        assert(size_);
        return Buffer()[size_ - 1];
    }

    /// Return size of vector.
    unsigned size() const { return size_; }

    /// Return capacity of vector.
    unsigned capacity() const { return capacity_; }

    /// Return whether vector is empty.
    bool empty() const { return size_ == 0; }

private:
    /// Return the buffer with right type.
    T* Buffer() const { return reinterpret_cast<T*>(buffer_); }

    /// Resize the vector and create/remove new elements as necessary.
    void resize(unsigned newSize, const T* src)
    {
        // If size shrinks, destruct the removed elements
        if (newSize < size_)
            DestructElements(Buffer() + newSize, size_ - newSize);
        else
        {
            // Allocate new buffer if necessary and copy the current elements
            if (newSize > capacity_)
            {
                if (!capacity_)
                    capacity_ = newSize;
                else
                {
                    while (capacity_ < newSize)
                        capacity_ += (capacity_ + 1) >> 1;
                }

                unsigned char* newBuffer = AllocateBuffer((unsigned)(capacity_ * sizeof(T)));
                if (buffer_)
                {
                    ConstructElements(reinterpret_cast<T*>(newBuffer), Buffer(), size_);
                    DestructElements(Buffer(), size_);
                    delete[] buffer_;
                }
                buffer_ = newBuffer;
            }

            // Initialize the new elements
            ConstructElements(Buffer() + size_, src, newSize - size_);
        }

        size_ = newSize;
    }

    /// Move a range of elements within the vector.
    void MoveRange(unsigned dest, unsigned src, unsigned count)
    {
        T* buffer = Buffer();
        if (src < dest)
        {
            for (unsigned i = count - 1; i < count; --i)
                buffer[dest + i] = buffer[src + i];
        }
        if (src > dest)
        {
            for (unsigned i = 0; i < count; ++i)
                buffer[dest + i] = buffer[src + i];
        }
    }

    /// Construct elements, optionally with source data.
    static void ConstructElements(T* dest, const T* src, unsigned count)
    {
        if (!src)
        {
            for (unsigned i = 0; i < count; ++i)
                new(dest + i) T();
        }
        else
        {
            for (unsigned i = 0; i < count; ++i)
                new(dest + i) T(*(src + i));
        }
    }

    /// Copy elements from one buffer to another.
    static void CopyElements(T* dest, const T* src, unsigned count)
    {
        while (count--)
            *dest++ = *src++;
    }

    // Call the elements' destructors.
    static void DestructElements(T* dest, unsigned count)
    {
        while (count--)
        {
            dest->~T();
            ++dest;
        }
    }
};

/// %Vector template class for POD types. Does not call constructors or destructors and uses block move.
template <class T> class PODVector : public VectorBase
{
public:
    typedef T ValueType;
    typedef RandomAccessIterator<T> Iterator;
    typedef RandomAccessConstIterator<T> ConstIterator;

    /// Construct empty.
    PODVector()
    {
    }

    /// Construct with initial size.
    explicit PODVector(unsigned size)
    {
        resize(size);
    }

    /// Construct with initial data.
    PODVector(const T* data, unsigned size)
    {
        resize(size);
        CopyElements(Buffer(), data, size);
    }

    /// Construct from another vector.
    PODVector(const PODVector<T>& vector)
    {
        *this = vector;
    }

    /// Destruct.
    ~PODVector()
    {
        delete[] buffer_;
    }

    /// Assign from another vector.
    PODVector<T>& operator =(const PODVector<T>& rhs)
    {
        resize(rhs.size_);
        CopyElements(Buffer(), rhs.Buffer(), rhs.size_);
        return *this;
    }

    /// Add-assign an element.
    PODVector<T>& operator +=(const T& rhs)
    {
        push_back(rhs);
        return *this;
    }

    /// Add-assign another vector.
    PODVector<T>& operator +=(const PODVector<T>& rhs)
    {
        push_back(rhs);
        return *this;
    }

    /// Add an element.
    PODVector<T> operator +(const T& rhs) const
    {
        PODVector<T> ret(*this);
        ret.push_back(rhs);
        return ret;
    }

    /// Add another vector.
    PODVector<T> operator +(const PODVector<T>& rhs) const
    {
        PODVector<T> ret(*this);
        ret.push_back(rhs);
        return ret;
    }

    /// Test for equality with another vector.
    bool operator ==(const PODVector<T>& rhs) const
    {
        if (rhs.size_ != size_)
            return false;

        T* buffer = Buffer();
        T* rhsBuffer = rhs.Buffer();
        for (unsigned i = 0; i < size_; ++i)
        {
            if (buffer[i] != rhsBuffer[i])
                return false;
        }

        return true;
    }

    /// Test for inequality with another vector.
    bool operator !=(const PODVector<T>& rhs) const
    {
        if (rhs.size_ != size_)
            return true;

        T* buffer = Buffer();
        T* rhsBuffer = rhs.Buffer();
        for (unsigned i = 0; i < size_; ++i)
        {
            if (buffer[i] != rhsBuffer[i])
                return true;
        }

        return false;
    }

    /// Return element at index.
    T& operator [](unsigned index)
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Return const element at index.
    const T& operator [](unsigned index) const
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Return element at index.
    T& At(unsigned index)
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Return const element at index.
    const T& At(unsigned index) const
    {
        assert(index < size_);
        return Buffer()[index];
    }

    /// Add an element at the end.
    void push_back(const T& value)
    {
        if (size_ < capacity_)
            ++size_;
        else
            resize(size_ + 1);
        back() = value;
    }

    /// Add another vector at the end.
    void push_back(const PODVector<T>& vector)
    {
        unsigned oldSize = size_;
        resize(size_ + vector.size_);
        CopyElements(Buffer() + oldSize, vector.Buffer(), vector.size_);
    }

    /// Remove the last element.
    void Pop()
    {
        if (size_)
            resize(size_ - 1);
    }

    /// insert an element at position.
    void insert(unsigned pos, const T& value)
    {
        if (pos > size_)
            pos = size_;

        unsigned oldSize = size_;
        resize(size_ + 1);
        MoveRange(pos + 1, pos, oldSize - pos);
        Buffer()[pos] = value;
    }

    /// insert another vector at position.
    void insert(unsigned pos, const PODVector<T>& vector)
    {
        if (pos > size_)
            pos = size_;

        unsigned oldSize = size_;
        resize(size_ + vector.size_);
        MoveRange(pos + vector.size_, pos, oldSize - pos);
        CopyElements(Buffer() + pos, vector.Buffer(), vector.size_);
    }

    /// insert an element by iterator.
    Iterator insert(const Iterator& dest, const T& value)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        insert(pos, value);

        return begin() + pos;
    }

    /// insert a vector by iterator.
    Iterator insert(const Iterator& dest, const PODVector<T>& vector)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        insert(pos, vector);

        return begin() + pos;
    }

    /// insert a vector partially by iterators.
    Iterator insert(const Iterator& dest, const ConstIterator& start, const ConstIterator& end)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        unsigned length = (unsigned)(end - start);
        resize(size_ + length);
        MoveRange(pos + length, pos, size_ - pos - length);
        CopyElements(Buffer() + pos, &(*start), length);

        return begin() + pos;
    }

    /// insert elements.
    Iterator insert(const Iterator& dest, const T* start, const T* end)
    {
        unsigned pos = (unsigned)(dest - begin());
        if (pos > size_)
            pos = size_;
        unsigned length = (unsigned)(end - start);
        resize(size_ + length);
        MoveRange(pos + length, pos, size_ - pos - length);

        T* destPtr = Buffer() + pos;
        for (const T* i = start; i != end; ++i)
            *destPtr++ = *i;

        return begin() + pos;
    }

    /// Erase a range of elements.
    void erase(unsigned pos, unsigned length = 1)
    {
        // Return if the range is illegal
        if (!length || pos + length > size_)
            return;

        MoveRange(pos, pos + length, size_ - pos - length);
        resize(size_ - length);
    }

    /// Erase an element by iterator. Return iterator to the next element.
    Iterator erase(const Iterator& it)
    {
        unsigned pos = (unsigned)(it - begin());
        if (pos >= size_)
            return end();
        erase(pos);

        return begin() + pos;
    }

    /// Erase a range by iterators. Return iterator to the next element.
    Iterator erase(const Iterator& start, const Iterator& end)
    {
        unsigned pos = (unsigned)(start - begin());
        if (pos >= size_)
            return end();
        unsigned length = (unsigned)(end - start);
        erase(pos, length);

        return begin() + pos;
    }

    /// Erase an element if found.
    bool Remove(const T& value)
    {
        Iterator i = find(value);
        if (i != end())
        {
            erase(i);
            return true;
        }
        else
            return false;
    }

    /// Clear the vector.
    void clear() { resize(0); }

    /// Resize the vector.
    void resize(unsigned newSize)
    {
        if (newSize > capacity_)
        {
            if (!capacity_)
                capacity_ = newSize;
            else
            {
                while (capacity_ < newSize)
                    capacity_ += (capacity_ + 1) >> 1;
            }

            unsigned char* newBuffer = AllocateBuffer((unsigned)(capacity_ * sizeof(T)));
            // Move the data into the new buffer and delete the old
            if (buffer_)
            {
                CopyElements(reinterpret_cast<T*>(newBuffer), Buffer(), size_);
                delete[] buffer_;
            }
            buffer_ = newBuffer;
        }

        size_ = newSize;
    }

    /// Set new capacity.
    void reserve(unsigned newCapacity)
    {
        if (newCapacity < size_)
            newCapacity = size_;

        if (newCapacity != capacity_)
        {
            unsigned char* newBuffer = 0;
            capacity_ = newCapacity;

            if (capacity_)
            {
                newBuffer = AllocateBuffer((unsigned)(capacity_ * sizeof(T)));
                // Move the data into the new buffer
                CopyElements(reinterpret_cast<T*>(newBuffer), Buffer(), size_);
            }

            // Delete the old buffer
            delete[] buffer_;
            buffer_ = newBuffer;
        }
    }

    /// Reallocate so that no extra memory is used.
    void Compact() { reserve(size_); }

    /// Return iterator to value, or to the end if not found.
    Iterator find(const T& value)
    {
        Iterator it = begin();
        while (it != end() && *it != value)
            ++it;
        return it;
    }

    /// Return const iterator to value, or to the end if not found.
    ConstIterator find(const T& value) const
    {
        ConstIterator it = begin();
        while (it != end() && *it != value)
            ++it;
        return it;
    }

    /// Return whether contains a specific value.
    bool Contains(const T& value) const { return find(value) != end(); }

    /// Return iterator to the beginning.
    Iterator begin() { return Iterator(Buffer()); }

    /// Return const iterator to the beginning.
    ConstIterator begin() const { return ConstIterator(Buffer()); }

    /// Return iterator to the end.
    Iterator end() { return Iterator(Buffer() + size_); }

    /// Return const iterator to the end.
    ConstIterator end() const { return ConstIterator(Buffer() + size_); }

    /// Return first element.
    T& front() { return Buffer()[0]; }

    /// Return const first element.
    const T& front() const { return Buffer()[0]; }

    /// Return last element.
    T& back()
    {
        assert(size_);
        return Buffer()[size_ - 1];
    }

    /// Return const last element.
    const T& back() const
    {
        assert(size_);
        return Buffer()[size_ - 1];
    }

    /// Return number of elements.
    unsigned size() const { return size_; }

    /// Return capacity of vector.
    unsigned capacity() const { return capacity_; }

    /// Return whether vector is empty.
    bool empty() const { return size_ == 0; }

private:
    /// Return the buffer with right type.
    T* Buffer() const { return reinterpret_cast<T*>(buffer_); }

    /// Move a range of elements within the vector.
    void MoveRange(unsigned dest, unsigned src, unsigned count)
    {
        if (count)
            memmove(Buffer() + dest, Buffer() + src, count * sizeof(T));
    }

    /// Copy elements from one buffer to another.
    static void CopyElements(T* dest, const T* src, unsigned count)
    {
        if (count)
            memcpy(dest, src, count * sizeof(T));
    }
};

}

namespace std
{

template <class T> typename YumeEngine::Vector<T>::ConstIterator begin(const YumeEngine::Vector<T>& v) { return v.begin(); }

template <class T> typename YumeEngine::Vector<T>::ConstIterator end(const YumeEngine::Vector<T>& v) { return v.end(); }

template <class T> typename YumeEngine::Vector<T>::Iterator begin(YumeEngine::Vector<T>& v) { return v.begin(); }

template <class T> typename YumeEngine::Vector<T>::Iterator end(YumeEngine::Vector<T>& v) { return v.end(); }

template <class T> typename YumeEngine::PODVector<T>::ConstIterator begin(const YumeEngine::PODVector<T>& v) { return v.begin(); }

template <class T> typename YumeEngine::PODVector<T>::ConstIterator end(const YumeEngine::PODVector<T>& v) { return v.end(); }

template <class T> typename YumeEngine::PODVector<T>::Iterator begin(YumeEngine::PODVector<T>& v) { return v.begin(); }

template <class T> typename YumeEngine::PODVector<T>::Iterator end(YumeEngine::PODVector<T>& v) { return v.end(); }

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif