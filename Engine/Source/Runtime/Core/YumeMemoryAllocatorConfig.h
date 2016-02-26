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
/// File : <Filename> YumeMemoryAllocatorConfig.h
/// Date : 8.28.2015
/// Comments :
///
///////////////////////////////////////////////////////////////////////////////////


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

#define SharedPtr boost::shared_ptr

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
