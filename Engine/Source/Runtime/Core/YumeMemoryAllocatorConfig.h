





























#ifndef __YumeMemoryAllocatorConfig_h__
#define __YumeMemoryAllocatorConfig_h__
//---------------------------------------------------------------------------------
#include "YumeHeaderPrefix.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	enum MemoryCategory
	{
		YUME_MEM_GENERAL = 0,
		YUME_MEM_RENDERERS = 1
	};
}
//---------------------------------------------------------------------------------
#include "YumeMallocObject.h"
#include "YumeMemoryAllocatorSTL.h"

//---------------------------------------------------------------------------------
#if YUME_MEMORY_ALLOCATOR == YUME_MEMORY_ALLOCATOR_NEDPOOLING

#include "YumeMemoryAllocatorNedPooling.h"
namespace YumeEngine
{
	template <MemoryCategory Cat> class YumeCategorisedAllocPolicy : public NedPoolingPolicy{};
	template <MemoryCategory Cat, size_t align = 0> class YumeCategorisedAlignAllocPolicy : public NedPoolingAlignedPolicy<align>{};
};
#endif
//---------------------------------------------------------------------------------
#if YUME_MEMORY_ALLOCATOR == YUME_MEMORY_ALLOCATOR_NED

#include "YumeMemoryAllocatorNed.h"
namespace YumeEngine
{
	template <MemoryCategory Cat> class YumeCategorisedAllocPolicy : public NedAllocPolicy{};
	template <MemoryCategory Cat, size_t align = 0> class YumeCategorisedAlignAllocPolicy : public NedAlignedAllocPolicy<align>{};
};

#endif

#if YUME_MEMORY_ALLOCATOR == YUME_MEMORY_ALLOCATOR_STD

#include "Core/YumeMemoryAllocatorStd.h"

namespace YumeEngine
{
	template <MemoryCategory Cat> class YumeCategorisedAllocPolicy : public StdAllocPolicy{};
	template <MemoryCategory Cat,size_t align = 0> class YumeCategorisedAlignAllocPolicy : public StdAlignedAllocPolicy<align>{};
};

#endif


namespace YumeEngine
{
	typedef YumeCategorisedAlignAllocPolicy<YumeEngine::YUME_MEM_GENERAL> YumeGeneralAllocPolicy;
	typedef YumeCategorisedAlignAllocPolicy<YumeEngine::YUME_MEM_RENDERERS> YumeRendererAllocPolicy;

	typedef YumeAllocatedObject<YumeGeneralAllocPolicy> YumeGeneralAllocatedObject;
	typedef YumeAllocatedObject<YumeRendererAllocPolicy> YumeRendererAllocatedObject;

	typedef YumeGeneralAllocatedObject GeneralObjAlloc;
	typedef YumeGeneralAllocatedObject LogObjAlloc;
	typedef YumeRendererAllocatedObject RenderObjAlloc;
	typedef YumeGeneralAllocatedObject DynamicLibAlloc;
}

namespace YumeEngine
{
	template<typename T>
	T* constructN(T* basePtr,size_t count)
	{
		for(size_t i = 0; i < count; ++i)
		{
			new ((void*)(basePtr+i)) T();
		}
		return basePtr;
	}
}
//---------------------------------------------------------------------------------


#if YUME_DEBUG_MODE
#	define YUME_DEALLOC(ptr, category) ::YumeEngine::YumeCategorisedAllocPolicy<category>::deallocateBytes((void*)ptr)
#	define YUME_ALLOC_T(T, count, category) static_cast<T*>(::YumeEngine::YumeCategorisedAllocPolicy<category>::allocateBytes(sizeof(T)*(count)))
#if defined(_MSC_VER)
#	define DebugNew new(_NORMAL_BLOCK, __FILE__, __LINE__)
#	define YumeAPINew new
#	define YumeAPIDelete delete
#else
#	define DebugNew new
#	define YumeAPINew DebugNew
#	define YumeAPIDelete delete
#endif
#else
#	define YUME_DEALLOC(ptr, category) ::YumeEngine::YumeCategorisedAllocPolicy<category>::deallocateBytes((void*)ptr)
#	define YumeAPINew new
#	define YumeAPIDelete delete
#endif

//---------------------------------------------------------------------------------
#include "YumeHeaderSuffix.h"
//---------------------------------------------------------------------------------
#endif
//~End of YumeMemoryAllocatorConfig.h
