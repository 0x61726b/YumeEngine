///////////////////////////////////////////////////////////////////////////////////
/// Yume Engine MIT License (MIT)

/// Copyright (c) 2015 Alperen Gezer

/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
/// 
/// File : YumeMallocObject.h
/// Date : 8.28.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeMemoryAllocatedObject_h__
#define __YumeMemoryAllocatedObject_h__
//---------------------------------------------------------------------------------
#include "YumeRequired.h"
#include "YumeHeaderPrefix.h"
//---------------------------------------------------------------------------------
#ifdef new
#  undef new
#endif
#ifdef delete
#  undef delete
#endif
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	template <class Alloc>
	class YumeAPIExport YumeAllocatedObject
	{
	public:
		YumeAllocatedObject()
		{ }

		~YumeAllocatedObject()
		{ }

		/// operator new, with debug line info
		void* operator new(size_t sz, const char* file, int line, const char* func)
		{
			return Alloc::allocateBytes(sz, file, line, func);
		}

		void* operator new(size_t sz)
		{
			return Alloc::allocateBytes(sz);
		}

		/// placement operator new
		void* operator new(size_t sz, void* ptr)
		{
			(void) sz;
			return ptr;
		}

		/// array operator new, with debug line info
		void* operator new[] ( size_t sz, const char* file, int line, const char* func )
		{
			return Alloc::allocateBytes(sz, file, line, func);
		}

		void* operator new[] ( size_t sz )
		{
			return Alloc::allocateBytes(sz);
		}

		void operator delete( void* ptr )
		{
			Alloc::deallocateBytes(ptr);
		}

		// Corresponding operator for placement delete (second param same as the first)
		void operator delete( void* ptr, void* )
		{
			Alloc::deallocateBytes(ptr);
		}

		// only called if there is an exception in corresponding 'new'
		void operator delete( void* ptr, const char* , int , const char*  )
		{
			Alloc::deallocateBytes(ptr);
		}

		void operator delete[] ( void* ptr )
		{
			Alloc::deallocateBytes(ptr);
		}


		void operator delete[] ( void* ptr, const char* , int , const char*  )
		{
			Alloc::deallocateBytes(ptr);
		}
	};
}
//---------------------------------------------------------------------------------
#include "YumeHeaderSuffix.h"
//---------------------------------------------------------------------------------
#endif
//~End of YumeMallocObject.h