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
/// File : YumePlatform.h
/// Date : 8.27.2015
/// Comments : Used to identify which platform we are running on.
///
///////////////////////////////////////////////////////////////////////////////////

///--------------------------------------------------------------------------------
#ifndef __YumePlatform_h__
#define __YumePlatform_h__
///--------------------------------------------------------------------------------
#include "YumeConfig.h"
///--------------------------------------------------------------------------------
namespace YumeEngine
{
		//--------------------------------------------------------------------------------
	#define YUME_PLATFORM_WIN32 1
	#define YUME_PLATFORM_LINUX 2
	#define YUME_PLATFORM_APPLE 3
	#define YUME_PLATFORM_APPLE_IOS 4
	#define YUME_PLATFORM_ANDROID 5
	#define YUME_PLATFORM_ANDROID 6
	#define YUME_PLATFORM_NACL 7
	//--------------------------------------------------------------------------------
	#define YUME_COMPILER_MSVC 1
	#define YUME_COMPILER_GNUC 2
	#define YUME_COMPILER_BORL 3
	#define YUME_COMPILER_WINSCW 4
	#define YUME_COMPILER_GCCE 5
	#define YUME_COMPILER_CLANG 6
	//--------------------------------------------------------------------------------
	#define YUME_SYSTEM_ARCH_64 1
	#define YUME_SYSTEM_ARCH_32 2
	//--------------------------------------------------------------------------------
	#if defined( __GCCE__ )
	#   define YUME_COMPILER YUME_COMPILER_GCCE
	#   define YUME_COMPILER_VERSION _MSC_VER
	#elif defined( __WINSCW__ )
	#   define YUME_COMPILER YUME_COMPILER_WINSCW
	#   define YUME_COMPILER_VERSION _MSC_VER
	#elif defined( _MSC_VER )
	#   define YUME_COMPILER YUME_COMPILER_MSVC
	#   define YUME_COMPILER_VERSION _MSC_VER
	#elif defined( __clang__ )
	#   define YUME_COMPILER YUME_COMPILER_CLANG
	#   define YUME_COMPILER_VERSION (((__clang_major__)*100) + \
			(__clang_minor__*10) + \
			__clang_patchlevel__)
	#elif defined( __GNUC__ )
	#   define YUME_COMPILER YUME_COMPILER_GNUC
	#   define YUME_COMPILER_VERSION (((__GNUC__)*100) + \
			(__GNUC_MINOR__*10) + \
			__GNUC_PATCHLEVEL__)
	#elif defined( __BORLANDC__ )
	#   define YUME_COMPILER YUME_COMPILER_BORL
	#   define YUME_COMPILER_VERSION __BCPLUSPLUS__
	#   define __FUNCTION__ __FUNC__ 
	#else
	#   pragma error "How did I end up here?"

	#endif
	//--------------------------------------------------------------------------------
	//
	//
	//
	//
	//--------------------------------------------------------------------------------
	#if YUME_COMPILER == YUME_COMPILER_MSVC
	#   if YUME_COMPILER_VERSION >= 1200
	#       define FORCEINLINE __forceinline
	#   endif
	#elif defined(__MINGW32__)
	#   if !defined(FORCEINLINE)
	#       define FORCEINLINE __inline
	#   endif
	#else
	#   define FORCEINLINE __inline
	#endif
	//--------------------------------------------------------------------------------
	//
	//Find out which platform we are running
	//	
	//--------------------------------------------------------------------------------
	#if defined( __WIN32__ ) || defined( _WIN32 )
	#   define YUME_PLATFORM YUME_PLATFORM_WIN32
	#elif defined(__ANDROID__)
	#	define YUME_PLATFORM YUME_PLATFORM_ANDROID
	#elif defined( __native_client__ ) 
	#   define YUME_PLATFORM OGRE_PLATFORM_NACL
	#   ifndef ARKEN_STATIC_LIB
	#       error OGRE must be built as static for NaCl
	#   endif
	#   ifdef YUME_BUILD_RENDERER_OGL
	#       error OpenGL is not supported on NaCl
	#   endif
	#else
	#   define YUME_PLATFORM YUME_PLATFORM_LINUX
	#endif
	//--------------------------------------------------------------------------------
	//
	//Find out the process Architecture
	//
	//--------------------------------------------------------------------------------
	#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
	#   define YUME_SYSTEM_ARCHITECTURE YUME_SYSTEM_ARCH_64
	#else
	#   define YUME_SYSTEM_ARCHITECTURE YUME_SYSTEM_ARCH_32
	#endif
	//--------------------------------------------------------------------------------
	//
	//
	//
	//
	//--------------------------------------------------------------------------------
	#if YUME_COMPILER == YUME_COMPILER_MSVC
	#   define YUME_DEPRECATED __declspec(deprecated)
	#elif YUME_COMPILER == YUME_COMPILER_GNUC || YUME_COMPILER == YUME_COMPILER_CLANG
	#   define OGRE_DEPRECATED __attribute__ ((deprecated))
	#else
	#   pragma message("WARNING: NO DEPRECATED FUNCTION IMPLEMENTED FOR THIS COMPILER SwiftRage")
	#   define OGRE_DEPRECATED
	#endif
	//Windows Settings
	#if YUME_PLATFORM == YUME_PLATFORM_WIN32 || YUME_PLATFORM == YUME_PLATFORM_WINRT

	#	if defined( YUME_STATIC_LIB )
			// Linux compilers don't have symbol import/export directives.
	#   	define YumeAPIExport
	#   	define YumeAPIPrivate
	#   else
	#   	if defined( YUME_NONCLIENT_BUILD )
	#       	define YumeAPIExport __declspec( dllexport )
	#   	else
	#           if defined( __MINGW32__ )
	#               define YumeAPIExport
	#           else
	#       	    define YumeAPIExport __declspec( dllimport )
	#           endif
	#   	endif
	#   	define YumeAPIPrivate
	#	endif
	#endif

	#define YUME_CPU_UNKNOWN    0
	#define YUME_CPU_X86        1
	#define YUME_CPU_PPC        2
	#define YUME_CPU_ARM        3


	#if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
		(defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
	#   define YUME_CPU YUME_CPU_X86

	#elif YUME_PLATFORM == YUME_PLATFORM_APPLE && defined(__BIG_ENDIAN__)
	#   define YUME_CPU YUME_CPU_PPC
	#elif YUME_PLATFORM == YUME_PLATFORM_APPLE
	#	define YUME_CPU YUME_CPU_X86
	#elif YUME_PLATFORM == YUME_PLATFORM_APPLE_IOS && (defined(__i386__) || defined(__x86_64__))
	#	define YUME_CPU YUME_CPU_X86
	#elif defined(__arm__)
	#	define YUME_CPU YUME_CPU_ARM
	#else
	#   define YUME_CPU YUME_CPU_UNKNOWN
	#endif

	#if YUME_COMPILER == YUME_COMPILER_MSVC
	#   define OGRE_ALIGNED_DECL(type, var, alignment)  __declspec(align(alignment)) type var

	#elif (YUME_COMPILER == YUME_COMPILER_GNUC) || (YUME_COMPILER == YUME_COMPILER_CLANG)
	#   define YUME_ALIGNED_DECL(type, var, alignment)  type var __attribute__((__aligned__(alignment)))

	#else
	#   define YUME_ALIGNED_DECL(type, var, alignment)  type var
	#endif

	#if YUME_CPU == YUME_CPU_X86
	#   define YUME_SIMD_ALIGNMENT  16

	#else
	#   define YUME_SIMD_ALIGNMENT  16
	#endif

	#define YUME_SIMD_ALIGNED_DECL(type, var)   YUME_ALIGNED_DECL(type, var, OGRE_SIMD_ALIGNMENT)
}
///--------------------------------------------------------------------------------
#endif
//~End of YumePlatform.h

