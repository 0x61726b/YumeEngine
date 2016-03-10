




























//---------------------------------------------------------------------------------
#include "YumeHeaders.h"
//#include "YumePlatformInformation.h"
#include "YumeRequired.h"
#include "YumeMemoryAllocatorNed.h"
//---------------------------------------------------------------------------------
#if YUME_MEMORY_ALLOCATOR == YUME_MEMORY_ALLOCATOR_NED
//---------------------------------------------------------------------------------
#define ABORT_ON_ASSERT_FAILURE 0
#include <nedmalloc.c>

namespace YumeEngine
{
	//---------------------------------------------------------------------
	void* NedAllocImpl::allocBytes(size_t count, 
		const char* file, int line, const char* func)
	{
		void* ptr = nedalloc::nedmalloc(count);
	//#if OGRE_MEMORY_TRACKER
	//	// this alloc policy doesn't do pools (yet, ned can do it)
	//	MemoryTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
	//#endif
		return ptr;
	}
	//---------------------------------------------------------------------
	void NedAllocImpl::deallocBytes(void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
//#if OGRE_MEMORY_TRACKER
//		MemoryTracker::get()._recordDealloc(ptr);
//#endif
		nedalloc::nedfree(ptr);
	}
	//---------------------------------------------------------------------
	void* NedAllocImpl::allocBytesAligned(size_t align, size_t count, 
		const char* file, int line, const char* func)
	{
		// default to platform SIMD alignment if none specified
		void* ptr =  align ? nedalloc::nedmemalign(align, count)
			: nedalloc::nedmemalign(YUME_SIMD_ALIGNMENT, count);
//#if OGRE_MEMORY_TRACKER
//		// this alloc policy doesn't do pools (yet, ned can do it)
//		MemoryTracker::get()._recordAlloc(ptr, count, 0, file, line, func);
//#endif
		return ptr;
	}
	//---------------------------------------------------------------------
	void NedAllocImpl::deallocBytesAligned(size_t align, void* ptr)
	{
		// deal with null
		if (!ptr)
			return;
//#if OGRE_MEMORY_TRACKER
//		// this alloc policy doesn't do pools (yet, ned can do it)
//		MemoryTracker::get()._recordDealloc(ptr);
//#endif
//		nedalloc::nedfree(ptr);
	}


}
//---------------------------------------------------------------------------------
#endif
//~End of YumeMemoryAllocatorNed.cpp
