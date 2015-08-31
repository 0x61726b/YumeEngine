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
/// File : YumeRequired.h
/// Date : 8.27.2015
/// Comments : 
///
///////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
#ifndef __YumeRequired_h__
#define __YumeRequired_h__
//---------------------------------------------------------------------------------
#include "YumePlatform.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	#define YUME_VERSION_MAJOR 0
	#define YUME_VERSION_MINOR 1
	#define YUME_VERSION_PATCH 0
	#define YUME_VERSION_SUFFIX ""
	#define YUME_VERSION_NAME "Chitanda"

	#define YUME_VERSION_NUMBER    ((YUME_VERSION_MAJOR << 16) | (YUME_VERSION_MINOR << 8) | YUME_VERSION_PATCH)

	class YumeRenderer;
	class YumeRenderTarget;
}
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
#include "YumeStdHeaders.h"
#include "YumeMemoryAllocatorConfig.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	typedef std::string _YumeStringBase;

	typedef _YumeStringBase YumeString;
}
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	//STL Containers
	template <typename T, typename A = YumeSTLAllocator<T, YumeGeneralAllocPolicy> >
	struct YumeDeque
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::deque<T, A> type;
		typedef typename std::deque<T, A>::iterator iterator;
		typedef typename std::deque<T, A>::const_iterator const_iterator;
#else
		typedef typename std::deque<T> type;
		typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;
#endif
	};

	template <typename T, typename A = YumeSTLAllocator<T, YumeGeneralAllocPolicy> >
	struct YumeVector
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::vector<T, A> type;
		typedef typename std::vector<T, A>::iterator iterator;
		typedef typename std::vector<T, A>::const_iterator const_iterator;
#else
		typedef typename std::vector<T> type;
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;
#endif
	};

	template <typename T, typename A = YumeSTLAllocator<T, YumeGeneralAllocPolicy> >
	struct YumeList
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::list<T, A> type;
		typedef typename std::list<T, A>::iterator iterator;
		typedef typename std::list<T, A>::const_iterator const_iterator;
#else
		typedef typename std::list<T> type;
		typedef typename std::list<T>::iterator iterator;
		typedef typename std::list<T>::const_iterator const_iterator;
#endif
	};

	template <typename T, typename P = std::less<T>, typename A = YumeSTLAllocator<T, YumeGeneralAllocPolicy> >
	struct YumeSet
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::set<T, P, A> type;
		typedef typename std::set<T, P, A>::iterator iterator;
		typedef typename std::set<T, P, A>::const_iterator const_iterator;
#else
		typedef typename std::set<T, P> type;
		typedef typename std::set<T, P>::iterator iterator;
		typedef typename std::set<T, P>::const_iterator const_iterator;
#endif
	};
	template <typename K, typename V, typename P = std::less<K>, typename A = YumeSTLAllocator<std::pair<const K, V>, YumeGeneralAllocPolicy> >
	struct YumeMap
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::map<K, V, P, A> type;
		typedef typename std::map<K, V, P, A>::iterator iterator;
		typedef typename std::map<K, V, P, A>::const_iterator const_iterator;
#else
		typedef typename std::map<K, V, P> type;
		typedef typename std::map<K, V, P>::iterator iterator;
		typedef typename std::map<K, V, P>::const_iterator const_iterator;
#endif
	};
	template <typename K, typename V, typename P = std::less<K>, typename A = YumeSTLAllocator<std::pair<const K, V>, YumeGeneralAllocPolicy> >
	struct YumeMultiMap
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::multimap<K, V, P, A> type;
		typedef typename std::multimap<K, V, P, A>::iterator iterator;
		typedef typename std::multimap<K, V, P, A>::const_iterator const_iterator;
#else
		typedef typename std::multimap<K, V, P> type;
		typedef typename std::multimap<K, V, P>::iterator iterator;
		typedef typename std::multimap<K, V, P>::const_iterator const_iterator;
#endif
	};
}

//---------------------------------------------------------------------------------
#endif
//~End of YumeConfig.h