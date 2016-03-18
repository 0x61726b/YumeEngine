#ifndef __StdHeaders_H__
#define __StdHeaders_H__


#include "EASTL/vector.h"
#include "EASTL/hash_map.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>

// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>
#include <unordered_map>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef __BORLANDC__
namespace YumeEngine
{
    using namespace std;
}
#endif

extern "C" {

#   include <sys/types.h>
#   include <sys/stat.h>

}

#if YUME_PLATFORM == YUME_PLATFORM_WIN32 || YUME_PLATFORM == YUME_PLATFORM_WINRT
#  undef min
#  undef max
#  if defined( __MINGW32__ )
#    include <unistd.h>
#  endif
#endif

#if YUME_PLATFORM == YUME_PLATFORM_LINUX || YUME_PLATFORM == YUME_PLATFORM_ANDROID
extern "C" {

#   include <unistd.h>
#   include <dlfcn.h>

}
#endif

#if YUME_PLATFORM == YUME_PLATFORM_APPLE || YUME_PLATFORM == YUME_PLATFORM_APPLE_IOS
extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
#   include <CoreFoundation/CoreFoundation.h>
}
#endif

//#if defined ( OGRE_GCC_VISIBILITY ) && ((OGRE_PLATFORM == OGRE_PLATFORM_APPLE && !__LP64__) && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
//#   pragma GCC visibility pop
//#endif
#endif
