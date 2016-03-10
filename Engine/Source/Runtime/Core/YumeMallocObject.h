




























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
		explicit YumeAllocatedObject()
		{ }

		~YumeAllocatedObject()
		{ }

		
		void* operator new(size_t sz, const char* file, int line, const char* func)
		{

			return Alloc::allocateBytes(sz, file, line, func);
		}

		void* operator new(size_t sz)
		{
			return Alloc::allocateBytes(sz);
		}

		
		void* operator new(size_t sz, void* ptr)
		{
			(void) sz;
			return ptr;
		}

		
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