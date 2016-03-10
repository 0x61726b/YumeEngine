




























//---------------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeAlignedAllocator.h"
#include "YumeBitPattern.h"

namespace YumeEngine
{
	//---------------------------------------------------------------------
	void* AlignedMemory::allocate(size_t size,size_t alignment)
	{
		assert(0 < alignment && alignment <= 128 && Bitwise::isPO2(alignment));

		unsigned char* p = new unsigned char[size + alignment];
		size_t offset = alignment - (size_t(p) & (alignment-1));

		unsigned char* result = p + offset;
		result[-1] = (unsigned char)offset;

		return result;
	}
	//---------------------------------------------------------------------
	void* AlignedMemory::allocate(size_t size)
	{
		return allocate(size,YUME_SIMD_ALIGNMENT);
	}
	//---------------------------------------------------------------------
	void AlignedMemory::deallocate(void* p)
	{
		if(p)
		{
			unsigned char* mem = (unsigned char*)p;
			mem = mem - mem[-1];
			delete[] mem;
		}
	}
}
