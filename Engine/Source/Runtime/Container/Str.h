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
#ifndef __YumeStr_h__
#define __YumeStr_h__
//----------------------------------------------------------------------------
#pragma once

#include "Vector.h"

#include <cstdarg>
#include <cstring>
#include <cctype>

namespace YumeEngine
{

static const int CONVERSION_BUFFER_LENGTH = 128;
static const int MATRIX_CONVERSION_BUFFER_LENGTH = 256;

class WString;

/// %String class.
class YumeAPIExport String
{
public:
    typedef RandomAccessIterator<char> Iterator;
    typedef RandomAccessConstIterator<char> ConstIterator;

    /// Construct empty.
    String() :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
    }

    /// Construct from another string.
    String(const String& str) :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
        *this = str;
    }

    /// Construct from a C string.
    String(const char* str) :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
        *this = str;
    }

    /// Construct from a C string.
    String(char* str) :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
        *this = (const char*)str;
    }

    /// Construct from a char array and length.
    String(const char* str, unsigned length) :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
        resize(length);
        CopyChars(buffer_, str, length);
    }

    /// Construct from a null-terminated wide character array.
    String(const wchar_t* str) :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
        SetUTF8FromWChar(str);
    }

    /// Construct from a null-terminated wide character array.
    String(wchar_t* str) :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
        SetUTF8FromWChar(str);
    }

    /// Construct from a wide character string.
    String(const WString& str);

    /// Construct from an integer.
    explicit String(int value);
    /// Construct from a short integer.
    explicit String(short value);
    /// Construct from a long integer.
    explicit String(long value);
    /// Construct from a long long integer.
    explicit String(long long value);
    /// Construct from an unsigned integer.
    explicit String(unsigned value);
    /// Construct from an unsigned short integer.
    explicit String(unsigned short value);
    /// Construct from an unsigned long integer.
    explicit String(unsigned long value);
    /// Construct from an unsigned long long integer.
    explicit String(unsigned long long value);
    /// Construct from a float.
    explicit String(float value);
    /// Construct from a double.
    explicit String(double value);
    /// Construct from a bool.
    explicit String(bool value);
    /// Construct from a character.
    explicit String(char value);
    /// Construct from a character and fill length.
    explicit String(char value, unsigned length);

    /// Construct from a convertable value.
    template <class T> explicit String(const T& value) :
        length_(0),
        capacity_(0),
        buffer_(&endZero)
    {
        *this = value.ToString();
    }

    /// Destruct.
    ~String()
    {
        if (capacity_)
            delete[] buffer_;
    }

    /// Assign a string.
    String& operator =(const String& rhs)
    {
        resize(rhs.length_);
        CopyChars(buffer_, rhs.buffer_, rhs.length_);

        return *this;
    }

    /// Assign a C string.
    String& operator =(const char* rhs)
    {
        unsigned rhsLength = CStringLength(rhs);
        resize(rhsLength);
        CopyChars(buffer_, rhs, rhsLength);

        return *this;
    }

    /// Add-assign a string.
    String& operator +=(const String& rhs)
    {
        unsigned oldLength = length_;
        resize(length_ + rhs.length_);
        CopyChars(buffer_ + oldLength, rhs.buffer_, rhs.length_);

        return *this;
    }

    /// Add-assign a C string.
    String& operator +=(const char* rhs)
    {
        unsigned rhsLength = CStringLength(rhs);
        unsigned oldLength = length_;
        resize(length_ + rhsLength);
        CopyChars(buffer_ + oldLength, rhs, rhsLength);

        return *this;
    }

    /// Add-assign a character.
    String& operator +=(char rhs)
    {
        unsigned oldLength = length_;
        resize(length_ + 1);
        buffer_[oldLength] = rhs;

        return *this;
    }

    /// Add-assign an integer.
    String& operator +=(int rhs);
    /// Add-assign a short integer.
    String& operator +=(short rhs);
    /// Add-assign a long integer.
    String& operator +=(long rhs);
    /// Add-assign a long long integer.
    String& operator +=(long long rhs);
    /// Add-assign an unsigned integer.
    String& operator +=(unsigned rhs);
    /// Add-assign a short unsigned integer.
    String& operator +=(unsigned short rhs);
    /// Add-assign a long unsigned integer.
    String& operator +=(unsigned long rhs);
    /// Add-assign a long long unsigned integer.
    String& operator +=(unsigned long long rhs);
    /// Add-assign a float.
    String& operator +=(float rhs);
    /// Add-assign a bool.
    String& operator +=(bool rhs);

    /// Add-assign an arbitraty type.
    template <class T> String operator +=(const T& rhs) { return *this += rhs.ToString(); }

    /// Add a string.
    String operator +(const String& rhs) const
    {
        String ret;
        ret.resize(length_ + rhs.length_);
        CopyChars(ret.buffer_, buffer_, length_);
        CopyChars(ret.buffer_ + length_, rhs.buffer_, rhs.length_);

        return ret;
    }

    /// Add a C string.
    String operator +(const char* rhs) const
    {
        unsigned rhsLength = CStringLength(rhs);
        String ret;
        ret.resize(length_ + rhsLength);
        CopyChars(ret.buffer_, buffer_, length_);
        CopyChars(ret.buffer_ + length_, rhs, rhsLength);

        return ret;
    }

    /// Add a character.
    String operator +(char rhs) const
    {
        String ret(*this);
        ret += rhs;

        return ret;
    }

    /// Test for equality with another string.
    bool operator ==(const String& rhs) const { return strcmp(c_str(), rhs.c_str()) == 0; }

    /// Test for inequality with another string.
    bool operator !=(const String& rhs) const { return strcmp(c_str(), rhs.c_str()) != 0; }

    /// Test if string is less than another string.
    bool operator <(const String& rhs) const { return strcmp(c_str(), rhs.c_str()) < 0; }

    /// Test if string is greater than another string.
    bool operator >(const String& rhs) const { return strcmp(c_str(), rhs.c_str()) > 0; }

    /// Test for equality with a C string.
    bool operator ==(const char* rhs) const { return strcmp(c_str(), rhs) == 0; }

    /// Test for inequality with a C string.
    bool operator !=(const char* rhs) const { return strcmp(c_str(), rhs) != 0; }

    /// Test if string is less than a C string.
    bool operator <(const char* rhs) const { return strcmp(c_str(), rhs) < 0; }

    /// Test if string is greater than a C string.
    bool operator >(const char* rhs) const { return strcmp(c_str(), rhs) > 0; }

    /// Return char at index.
    char& operator [](unsigned index)
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Return const char at index.
    const char& operator [](unsigned index) const
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Return char at index.
    char& At(unsigned index)
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Return const char at index.
    const char& At(unsigned index) const
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Replace all occurrences of a character.
    void Replace(char replaceThis, char replaceWith, bool caseSensitive = true);
    /// Replace all occurrences of a string.
    void Replace(const String& replaceThis, const String& replaceWith, bool caseSensitive = true);
    /// Replace a substring.
    void Replace(unsigned pos, unsigned length, const String& replaceWith);
    /// Replace a substring with a C string.
    void Replace(unsigned pos, unsigned length, const char* replaceWith);
    /// Replace a substring by iterators.
    Iterator Replace(const Iterator& start, const Iterator& end, const String& replaceWith);
    /// Return a string with all occurrences of a character replaced.
    String Replaced(char replaceThis, char replaceWith, bool caseSensitive = true) const;
    /// Return a string with all occurrences of a string replaced.
    String Replaced(const String& replaceThis, const String& replaceWith, bool caseSensitive = true) const;
    /// Append a string.
    String& append(const String& str);
    /// Append a C string.
    String& append(const char* str);
    /// Append a character.
    String& append(char c);
    /// Append characters.
    String& append(const char* str, unsigned length);
    /// Insert a string.
    void Insert(unsigned pos, const String& str);
    /// Insert a character.
    void Insert(unsigned pos, char c);
    /// Insert a string by iterator.
    Iterator Insert(const Iterator& dest, const String& str);
    /// Insert a string partially by iterators.
    Iterator Insert(const Iterator& dest, const Iterator& start, const Iterator& end);
    /// Insert a character by iterator.
    Iterator Insert(const Iterator& dest, char c);
    /// Erase a substring.
    void Erase(unsigned pos, unsigned length = 1);
    /// Erase a character by iterator.
    Iterator Erase(const Iterator& it);
    /// Erase a substring by iterators.
    Iterator Erase(const Iterator& start, const Iterator& end);
    /// Resize the string.
    void resize(unsigned newLength);
    /// Set new capacity.
    void Reserve(unsigned newCapacity);
    /// Reallocate so that no extra memory is used.
    void Compact();
    /// Clear the string.
    void Clear();
    /// Swap with another string.
    void Swap(String& str);

    /// Return iterator to the beginning.
    Iterator Begin() { return Iterator(buffer_); }

    /// Return const iterator to the beginning.
    ConstIterator Begin() const { return ConstIterator(buffer_); }

    /// Return iterator to the end.
    Iterator End() { return Iterator(buffer_ + length_); }

    /// Return const iterator to the end.
    ConstIterator End() const { return ConstIterator(buffer_ + length_); }

    /// Return first char, or 0 if empty.
    char Front() const { return buffer_[0]; }

    /// Return last char, or 0 if empty.
    char Back() const { return length_ ? buffer_[length_ - 1] : buffer_[0]; }

    /// Return a substring from position to end.
    String substr(unsigned pos) const;
    /// Return a substring with length from position.
    String substr(unsigned pos, unsigned length) const;
    /// Return string with whitespace trimmed from the beginning and the end.
    String Trimmed() const;
    /// Return string in uppercase.
    String ToUpper() const;
    /// Return string in lowercase.
    String ToLower() const;
    /// Return substrings split by a separator char. By default don't return empty strings.
    Vector<String> Split(char separator, bool keepEmptyStrings = false) const;
    /// Join substrings with a 'glue' string.
    void Join(const Vector<String>& subStrings, const String& glue);
    /// Return index to the first occurrence of a string, or NPOS if not found.
    unsigned find(const String& str, unsigned startPos = 0, bool caseSensitive = true) const;
    /// Return index to the first occurrence of a character, or NPOS if not found.
    unsigned find(char c, unsigned startPos = 0, bool caseSensitive = true) const;
    /// Return index to the last occurrence of a string, or NPOS if not found.
    unsigned find_last_of(const String& str, unsigned startPos = NPOS, bool caseSensitive = true) const;
    /// Return index to the last occurrence of a character, or NPOS if not found.
    unsigned find_last_of(char c, unsigned startPos = NPOS, bool caseSensitive = true) const;
    /// Return whether starts with a string.
    bool StartsWith(const String& str, bool caseSensitive = true) const;
    /// Return whether ends with a string.
    bool EndsWith(const String& str, bool caseSensitive = true) const;

    /// Return the C string.
    const char* c_str() const { return buffer_; }

    /// Return length.
    unsigned length() const { return length_; }

    /// Return buffer capacity.
    unsigned Capacity() const { return capacity_; }

    /// Return whether the string is empty.
    bool empty() const { return length_ == 0; }

    /// Return comparison result with a string.
    int Compare(const String& str, bool caseSensitive = true) const;
    /// Return comparison result with a C string.
    int Compare(const char* str, bool caseSensitive = true) const;

    /// Return whether contains a specific occurrence of a string.
    bool Contains(const String& str, bool caseSensitive = true) const { return find(str, 0, caseSensitive) != NPOS; }

    /// Return whether contains a specific character.
    bool Contains(char c, bool caseSensitive = true) const { return find(c, 0, caseSensitive) != NPOS; }

    /// Construct UTF8 content from Latin1.
    void SetUTF8FromLatin1(const char* str);
    /// Construct UTF8 content from wide characters.
    void SetUTF8FromWChar(const wchar_t* str);
    /// Calculate number of characters in UTF8 content.
    unsigned LengthUTF8() const;
    /// Return byte offset to char in UTF8 content.
    unsigned ByteOffsetUTF8(unsigned index) const;
    /// Return next Unicode character from UTF8 content and increase byte offset.
    unsigned NextUTF8Char(unsigned& byteOffset) const;
    /// Return Unicode character at index from UTF8 content.
    unsigned AtUTF8(unsigned index) const;
    /// Replace Unicode character at index from UTF8 content.
    void ReplaceUTF8(unsigned index, unsigned unicodeChar);
    /// Append Unicode character at the end as UTF8.
    String& AppendUTF8(unsigned unicodeChar);
    /// Return a UTF8 substring from position to end.
    String SubstringUTF8(unsigned pos) const;
    /// Return a UTF8 substring with length from position.
    String SubstringUTF8(unsigned pos, unsigned length) const;

    /// Return hash value for HashSet & HashMap.
    unsigned ToHash() const
    {
        unsigned hash = 0;
        const char* ptr = buffer_;
        while (*ptr)
        {
            hash = *ptr + (hash << 6) + (hash << 16) - hash;
            ++ptr;
        }

        return hash;
    }

    /// Return substrings split by a separator char. By default don't return empty strings.
    static Vector<String> Split(const char* str, char separator, bool keepEmptyStrings = false);
    /// Return a string by joining substrings with a 'glue' string.
    static String Joined(const Vector<String>& subStrings, const String& glue);
    /// Encode Unicode character to UTF8. Pointer will be incremented.
    static void EncodeUTF8(char*& dest, unsigned unicodeChar);
    /// Decode Unicode character from UTF8. Pointer will be incremented.
    static unsigned DecodeUTF8(const char*& src);
#ifdef _WIN32
    /// Encode Unicode character to UTF16. Pointer will be incremented.
    static void EncodeUTF16(wchar_t*& dest, unsigned unicodeChar);
    /// Decode Unicode character from UTF16. Pointer will be incremented.
    static unsigned DecodeUTF16(const wchar_t*& src);
    #endif

    /// Return length of a C string.
    static unsigned CStringLength(const char* str)
    {
        if (!str)
            return 0;
#ifdef _MSC_VER
        return (unsigned)strlen(str);
#else
        const char* ptr = str;
        while (*ptr)
            ++ptr;
        return (unsigned)(ptr - str);
#endif
    }

    /// Append to string using formatting.
    String& AppendWithFormat(const char* formatString, ...);
    /// Append to string using variable arguments.
    String& AppendWithFormatArgs(const char* formatString, va_list args);

    /// Compare two C strings.
    static int Compare(const char* str1, const char* str2, bool caseSensitive);

    /// Position for "not found."
    static const unsigned NPOS = 0xffffffff;
    /// Initial dynamic allocation size.
    static const unsigned MIN_CAPACITY = 8;
    /// Empty string.
    static const String EMPTY;

private:
    /// Move a range of characters within the string.
    void MoveRange(unsigned dest, unsigned src, unsigned count)
    {
        if (count)
            memmove(buffer_ + dest, buffer_ + src, count);
    }

    /// Copy chars from one buffer to another.
    static void CopyChars(char* dest, const char* src, unsigned count)
    {
#ifdef _MSC_VER
        if (count)
            memcpy(dest, src, count);
#else
        char* end = dest + count;
        while (dest != end)
        {
            *dest = *src;
            ++dest;
            ++src;
        }
#endif
    }

    /// Replace a substring with another substring.
    void Replace(unsigned pos, unsigned length, const char* srcStart, unsigned srcLength);

    /// String length.
    unsigned length_;
    /// Capacity, zero if buffer not allocated.
    unsigned capacity_;
    /// String buffer, null if not allocated.
    char* buffer_;

    /// End zero for empty strings.
    static char endZero;
};

/// Add a string to a C string.
inline String operator +(const char* lhs, const String& rhs)
{
    String ret(lhs);
    ret += rhs;
    return ret;
}

/// Add a string to a wide char C string.
inline String operator +(const wchar_t* lhs, const String& rhs)
{
    String ret(lhs);
    ret += rhs;
    return ret;
}

/// Wide character string. Only meant for converting from String and passing to the operating system where necessary.
class YumeAPIExport WString
{
public:
    /// Construct empty.
    WString();
    /// Construct from a string.
    WString(const String& str);
    /// Destruct.
    ~WString();

    /// Return char at index.
    wchar_t& operator [](unsigned index)
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Return const char at index.
    const wchar_t& operator [](unsigned index) const
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Return char at index.
    wchar_t& At(unsigned index)
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Return const char at index.
    const wchar_t& At(unsigned index) const
    {
        assert(index < length_);
        return buffer_[index];
    }

    /// Resize the string.
    void resize(unsigned newLength);

    /// Return whether the string is empty.
    bool empty() const { return length_ == 0; }

    /// Return length.
    unsigned length() const { return length_; }

    /// Return character data.
    const wchar_t* c_str() const { return buffer_; }

private:
    /// String length.
    unsigned length_;
    /// String buffer, null if not allocated.
    wchar_t* buffer_;
};

}
#endif