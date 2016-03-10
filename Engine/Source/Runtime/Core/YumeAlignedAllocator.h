




























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

