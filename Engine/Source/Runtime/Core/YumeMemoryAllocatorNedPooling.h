




























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

