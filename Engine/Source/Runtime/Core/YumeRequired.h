//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename> YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------

#ifndef __YumeRequired_h__
#define __YumeRequired_h__
//---------------------------------------------------------------------------------
#include "YumePlatform.h"
#include "PlatformImpl.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
#define YUME_VERSION_MAJOR 0
#define YUME_VERSION_MINOR 1
#define YUME_VERSION_PATCH 0
#define YUME_VERSION_SUFFIX ""
#define YUME_VERSION_NAME "Chitanda"

#define YUME_VERSION_NUMBER    ((YUME_VERSION_MAJOR << 16) | (YUME_VERSION_MINOR << 8) | YUME_VERSION_PATCH)
}
//---------------------------------------------------------------------------------
#include "YumeStdHeaders.h"
#include "YumeMemoryAllocatorConfig.h"
#include "Core/SharedPtr.h"

//---------------------------------------------------------------------------------
namespace YumeEngine
{
	typedef std::string _YumeStringBase;
	typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _YumeStreamBase;

	typedef std::string YumeString;
	typedef _YumeStreamBase YumeStringStream;

	static YumeString EmptyString = "";

	class YumeEngine3D;
	class YumeRenderer;
	class YumeRHI;
	class YumeResourceManager;
	class YumeIO;
	class YumeTime;
	class YumeWorkQueue;
	class YumeEnvironment;
	class YumeDebugRenderer;
	class YumeInput;
	class YumeObjectFactory;

	class Vector2;
	class Vector3;
	class Vector4;

	class Matrix3x4;
	class Matrix3;
	class Matrix4;


	class Quaternion;
}
//---------------------------------------------------------------------------------
#include "Math/YumeHash.h"
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	//STL Containers
	template <typename T,typename A = YumeSTLAllocator<T,YumeGeneralAllocPolicy> >
	struct YumeDeque
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::deque<T,A> type;
		typedef typename std::deque<T,A>::iterator iterator;
		typedef typename std::deque<T,A>::const_iterator const_iterator;
#else
		typedef typename std::deque<T> type;
		typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;
#endif
	};

	template <typename T>
	struct YumeVector
	{
		typedef typename eastl::vector<T> type;
		typedef typename eastl::vector<T>::iterator iterator;
		typedef typename eastl::vector<T>::const_iterator const_iterator;
	};

	template <typename T,typename A = YumeSTLAllocator<T,YumeGeneralAllocPolicy> >
	struct YumeList
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::list<T,A> type;
		typedef typename std::list<T,A>::iterator iterator;
		typedef typename std::list<T,A>::const_iterator const_iterator;
#else
		typedef typename std::list<T> type;
		typedef typename std::list<T>::iterator iterator;
		typedef typename std::list<T>::const_iterator const_iterator;
#endif
	};

	template <typename T,typename P = std::less<T>,typename A = YumeSTLAllocator<T,YumeGeneralAllocPolicy> >
	struct YumeSet
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::set<T,P,A> type;
		typedef typename std::set<T,P,A>::iterator iterator;
		typedef typename std::set<T,P,A>::const_iterator const_iterator;
#else
		typedef typename std::set<T, P> type;
		typedef typename std::set<T, P>::iterator iterator;
		typedef typename std::set<T, P>::const_iterator const_iterator;
#endif
	};
	template <typename K,typename V >
	struct YumeMap
	{
		typedef typename std::map<K, V> type;
		typedef typename std::map<K, V>::iterator iterator;
		typedef typename std::map<K, V>::const_iterator const_iterator;
	};
	template <typename K,typename V,typename P = std::less<K>,typename A = YumeSTLAllocator<std::pair<const K,V>,YumeGeneralAllocPolicy> >
	struct YumeMultiMap
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::multimap<K,V,P,A> type;
		typedef typename std::multimap<K,V,P,A>::iterator iterator;
		typedef typename std::multimap<K,V,P,A>::const_iterator const_iterator;
#else
		typedef typename std::multimap<K, V, P> type;
		typedef typename std::multimap<K, V, P>::iterator iterator;
		typedef typename std::multimap<K, V, P>::const_iterator const_iterator;
#endif
	};

	template <typename K,typename V,typename H = std::hash<K> >
	struct EastlHashMap
	{
		typedef typename std::unordered_map<K,V,H> type;
		typedef typename std::unordered_map<K,V,H>::iterator iterator;
		typedef typename std::unordered_map<K,V,H>::const_iterator const_iterator;
	};

	typedef YumeVector<YumeString>::type StringVector;

#define YUME_SAFE_RELEASE(p) if(p) { p->Release(); p = 0; }
}

#if YUME_PLATFORM != YUME_PLATFORM_APPLE
#define YUME_SSE
#endif

#include <boost/shared_array.hpp>

#include "Core/YumeBase.h"

struct YumeAPIExport GlobalSystems
{
	YumeEngine::YumeEngine3D*								pEngine;
	YumeEngine::YumeRHI*									pRHI;
	YumeEngine::YumeRenderer*								pRenderer;
	YumeEngine::YumeResourceManager*						pResourceManager;
	YumeEngine::YumeIO*										pIO;
	YumeEngine::YumeTime*									pTimer;
	YumeEngine::YumeWorkQueue*								pWorkSystem;
	YumeEngine::YumeEnvironment*							pEnv;
	YumeEngine::YumeDebugRenderer*							pDebugRenderer;
	YumeEngine::YumeInput*									pInput;
	YumeEngine::YumeObjectFactory*							pObjFactory;
};

extern YumeAPIExport GlobalSystems* gYume;
//---------------------------------------------------------------------------------
#endif
//~End of YumeRequired.h