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
/// File : <Filename> YumeMemoryAllocatorNedPooling.h
/// Date : 8.28.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeAlignedAllocator_h__
#define __YumeAlignedAllocator_h__

#include "YumeRequired.h"

namespace YumeEngine
{
	class YumeAPIExport AlignedMemory
	{
	public:
		/** Allocate memory with given alignment.
			@param
			size The size of memory need to allocate.
			@param
			alignment The alignment of result pointer, must be power of two
			and in range [1, 128].
			@return
			The allocated memory pointer.
			@par
			On failure, exception will be throw.
			*/
		static void* allocate(size_t size,size_t alignment);

		/** Allocate memory with default platform dependent alignment.
			@remarks
			The default alignment depend on target machine, this function
			guarantee aligned memory according with SIMD processing and
			cache boundary friendly.
			@param
			size The size of memory need to allocate.
			@return
			The allocated memory pointer.
			@par
			On failure, exception will be throw.
			*/
		static void* allocate(size_t size);

		/** Deallocate memory that allocated by this class.
			@param
			p Pointer to the memory allocated by this class or <b>NULL</b> pointer.
			@par
			On <b>NULL</b> pointer, nothing happen.
			*/
		static void deallocate(void* p);
	};


}

//---------------------------------------------------------------------------------
#endif
//~End of 

