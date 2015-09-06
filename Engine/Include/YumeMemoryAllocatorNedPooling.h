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
#ifndef __YumeMemoryAllocatorNedPooling_h__
#define __YumeMemoryAllocatorNedPooling_h__

#if YUME_MEMORY_ALLOCATOR == YUME_MEMORY_ALLOCATOR_NEDPOOLING

//---------------------------------------------------------------------------------
#include "YumeHeaderPrefix.h"
//---------------------------------------------------------------------------------

namespace YumeEngine
{
	class YumeAPIExport NedPoolingImpl
	{
	public:
		static void* allocBytes(size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytes(void* ptr);
		static void* allocBytesAligned(size_t align, size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytesAligned(size_t align, void* ptr);

	};

	class YumeAPIExport NedPoolingPolicy
	{
	public:
		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return NedPoolingImpl::allocBytes(count, file, line, func);
		}
		static inline void deallocateBytes(void* ptr)
		{
			NedPoolingImpl::deallocBytes(ptr);
		}
		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}

	private:
		// No instantiation
		NedPoolingPolicy()
		{ }
	};

	template <size_t Alignment = 0>
	class NedPoolingAlignedPolicy
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment
			[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return NedPoolingImpl::allocBytesAligned(Alignment, count, file, line, func);
		}

		static inline void deallocateBytes(void* ptr)
		{
			NedPoolingImpl::deallocBytesAligned(Alignment, ptr);
		}

		/// Get the maximum size of a single allocation
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}
	private:
		// no instantiation allowed
		NedPoolingAlignedPolicy()
		{ }
	};
}

#endif

//---------------------------------------------------------------------------------
#endif
//~End of 

