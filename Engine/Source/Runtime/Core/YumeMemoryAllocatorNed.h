




























//---------------------------------------------------------------------------------
#ifndef __YumeMemoryAllocatorNed_h__
#define __YumeMemoryAllocatorNed_h__

//---------------------------------------------------------------------------------
#include "YumeHeaderPrefix.h"
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
#if YUME_MEMORY_ALLOCATOR == YUME_MEMORY_ALLOCATOR_NED
//---------------------------------------------------------------------------------


namespace YumeEngine
{
	class YumeAPIExport NedAllocImpl
	{
	public:
		static void* allocBytes(size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytes(void* ptr);
		static void* allocBytesAligned(size_t align, size_t count, 
			const char* file, int line, const char* func);
		static void deallocBytesAligned(size_t align, void* ptr);
	};

	class YumeAPIExport NedAllocPolicy
	{
	public:
		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return NedAllocImpl::allocBytes(count, file, line, func);
		}
		static inline void deallocateBytes(void* ptr)
		{
			NedAllocImpl::deallocBytes(ptr);
		}
		
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}
	private:
		// No instantiation
		NedAllocPolicy()
		{ }
	};

	template <size_t Alignment = 0>
	class NedAlignedAllocPolicy
	{
	public:
		// compile-time check alignment is available.
		typedef int IsValidAlignment
			[Alignment <= 128 && ((Alignment & (Alignment-1)) == 0) ? +1 : -1];

		static inline void* allocateBytes(size_t count, 
			const char* file = 0, int line = 0, const char* func = 0)
		{
			return NedAllocImpl::allocBytesAligned(Alignment, count, file, line, func);
		}

		static inline void deallocateBytes(void* ptr)
		{
			NedAllocImpl::deallocBytesAligned(Alignment, ptr);
		}

		
		static inline size_t getMaxAllocationSize()
		{
			return std::numeric_limits<size_t>::max();
		}
	private:
		// no instantiation allowed
		NedAlignedAllocPolicy()
		{ }
	};
}
//---------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------
#include "YumeHeaderSuffix.h"
//---------------------------------------------------------------------------------
#endif
//~End of YumeMemoryAllocatorNed.h