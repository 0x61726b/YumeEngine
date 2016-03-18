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
#include "YumeHeaders.h"
#include "YumeAlloc.h"

#ifndef __YUMEALLOC_IMPL_H__
#define __YUMEALLOC_IMPL_H__


_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new(size_t size,const std::nothrow_t&) throw()
{
	void* const p = std::malloc(size);
	return p;
}


void operator delete(void* p,const std::nothrow_t&) throw()
{
	if(p) // The standard specifies that 'delete NULL' is a valid operation.
	{
		std::free(p);
	}
}


_Ret_maybenull_ _Post_writable_byte_size_(size) void* operator new[](size_t size,const std::nothrow_t&) throw()
{
	void* const p = std::malloc(size);
	return p;
}


void operator delete[](void* p,const std::nothrow_t&) throw()
{
	if(p)
	{
		std::free(p);
	}
}


_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new(size_t size) EA_THROW_SPEC_NEW(std::bad_alloc)
{
	void *mem;
	mem = malloc(size);


#if (_HAS_EXCEPTIONS == 1)
	if(mem == NULL)
	{
		throw std::bad_alloc();
	}
#endif

	return mem;
}


_Ret_notnull_ _Post_writable_byte_size_(size) void* operator new[](size_t size) EA_THROW_SPEC_NEW(std::bad_alloc)
{
	void *mem;
	mem = malloc(size);


#if !defined(EA_COMPILER_NO_EXCEPTIONS)
	if(mem == NULL)
	{
		throw std::bad_alloc();
	}
#endif

	return mem;
}


#ifdef _DEBUG
void* operator new[](size_t size,const char* name,int flags,unsigned debugFlags,const char* file,int line)
#else
void* operator new[](size_t size,const char* /*name*/,int flags,unsigned /*debugFlags*/,const char* /*file*/,int /*line*/)
#endif
{
	return std::malloc(size);
}


#ifdef EA_DEBUG
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
#else
void* operator new[](size_t size,size_t alignment,size_t alignmentOffset,const char* /*name*/,int flags,unsigned /*debugFlags*/,const char* /*file*/,int /*line*/)
#endif
{
	return _aligned_malloc(size,alignment);
}

// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
void* operator new(size_t size,size_t alignment)
{
	return _aligned_malloc(size,alignment);
}

// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
void* operator new(size_t size,size_t alignment,const std::nothrow_t&) throw()
{
	return _aligned_malloc(size,alignment);
}

// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
void* operator new[](size_t size,size_t alignment)
{
	return _aligned_malloc(size,alignment);
}

// Used by GCC when you make new objects of classes with >= N bit alignment (with N depending on the compiler).
void* operator new[](size_t size,size_t alignment,const std::nothrow_t&) throw()
{
	return _aligned_malloc(size,alignment);
}

#endif