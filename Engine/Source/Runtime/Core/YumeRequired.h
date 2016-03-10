



























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
//---------------------------------------------------------------------------------
namespace YumeEngine
{
	typedef std::string _YumeStringBase;
	typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _YumeStreamBase;

	typedef std::string YumeString;
	typedef _YumeStreamBase YumeStringStream;

	static YumeString EmptyString = "";

	typedef unsigned char yuchar;
	typedef unsigned short yushort;
	typedef unsigned int yuint;
	typedef unsigned long yulong;

	typedef float Real;

	//Math predefinitions
	class Radian;
	class Degree;
	class Angle;
	class Math;
	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix3;
	class Matrix4;
	class Quaternion;
	class Ray;
	class AxisAlignedBox;
	class Sphere;
	class Plane;
	class PlaneBoundedVolume;
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

	template <typename T,typename A = YumeSTLAllocator<T,YumeGeneralAllocPolicy> >
	struct YumeVector
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::vector<T,A> type;
		typedef typename std::vector<T,A>::iterator iterator;
		typedef typename std::vector<T,A>::const_iterator const_iterator;
#else
		typedef typename std::vector<T> type;
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;
#endif
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
	template <typename K,typename V,typename P = std::less<K>,typename A = YumeSTLAllocator<std::pair<const K,V>,YumeGeneralAllocPolicy> >
	struct YumeMap
	{
#if YUME_CONTAINERS_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::map<K,V,P,A> type;
		typedef typename std::map<K,V,P,A>::iterator iterator;
		typedef typename std::map<K,V,P,A>::const_iterator const_iterator;
#else
		typedef typename std::map<K, V, P> type;
		typedef typename std::map<K, V, P>::iterator iterator;
		typedef typename std::map<K, V, P>::const_iterator const_iterator;
#endif
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

	typedef YumeVector<YumeString>::type StringVector;

#define YUME_SAFE_RELEASE(p) if(p) { p->Release(); p = 0; }
}

#if YUME_PLATFORM != YUME_PLATFORM_APPLE
#define YUME_SSE
#endif

#include <boost/shared_array.hpp>

//---------------------------------------------------------------------------------
#endif
//~End of YumeRequired.h