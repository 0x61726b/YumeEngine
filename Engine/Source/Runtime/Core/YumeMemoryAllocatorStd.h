




























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

