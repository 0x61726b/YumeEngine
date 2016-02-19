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
/// File : YumeMemoryAllocatorNedPooling.h
/// Date : 8.28.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeMemoryAllocatorStd_h__
#define __YumeMemoryAllocatorStd_h__

#if YUME_MEMORY_ALLOCATOR == YUME_MEMORY_ALLOCATOR_STD

//---------------------------------------------------------------------------------
#include "YumeHeaderPrefix.h"
#include "YumeAlignedAllocator.h"
//---------------------------------------------------------------------------------

#include <memory>
#include <limits>


namespace YumeEngine
{
	class YumeAPIExport StdAllocPolicy
	{
	public:
		static inline void* allocateBytes(size_t count, 
			const char*  = 0, int  = 0, const char* = 0
            )
		{
			void* ptr = malloc(count);
			return ptr;
		}

		static inline void deallocateBytes(void* ptr)
		{
			free(ptr);
		}

		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}
	private:
		// no instantiation
		StdAllocPolicy()
		{ }
	};

	/**	A "standard" allocation policy for use with AllocatedObject and 
		STLAllocator, which aligns memory at a given boundary (which should be
		a power of 2). This is the class that actually does the allocation
		and deallocation of physical memory, and is what you will want to 
		provide a custom version of if you wish to change how memory is allocated.
		@par
		This class just delegates to the global malloc/free, via AlignedMemory.
		@note
		template parameter Alignment equal to zero means use default
		platform dependent alignment.

	*/
	template <size_t Alignment = 0>
	class StdAlignedAllocPolicy
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment
			[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

		static inline void* allocateBytes(size_t count, 
			const char*  = 0, int  = 0, const char* = 0
            )
		{
			void* ptr = Alignment ? AlignedMemory::allocate(count, Alignment)
				: AlignedMemory::allocate(count);
			return ptr;
		}

		static inline void deallocateBytes(void* ptr)
		{
			AlignedMemory::deallocate(ptr);
		}

		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}
	private:
		// No instantiation
		StdAlignedAllocPolicy()
		{ }
	};
}

#endif

//---------------------------------------------------------------------------------
#endif
//~End of 

