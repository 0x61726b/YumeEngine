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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"

#include "YumeDynamicLibrary.h"

#include "Logging/logging.h"

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>
#endif

#if YUME_PLATFORM == YUME_PLATFORM_APPLE
#   include <dlfcn.h>
#endif



namespace YumeEngine
{
	 //-----------------------------------------------------------------------
    YumeDynamicLibrary::YumeDynamicLibrary( const YumeString& name )
    {
        mName = name;
        mInst = NULL;
    }

    //-----------------------------------------------------------------------
    YumeDynamicLibrary::~YumeDynamicLibrary()
    {
    }

    //-----------------------------------------------------------------------
    bool YumeDynamicLibrary::Load()
    {
        // Log library load
        YUMELOG_INFO("Loading dynamic library " << mName << "....");

		YumeString name = mName;
#if YUME_PLATFORM == YUME_PLATFORM_LINUX
        // dlopen() does not add .so to the filename, like windows does for .dll
	if (name.find(".so") == std::string::npos)
           name += ".so";
#elif YUME_PLATFORM == YUME_PLATFORM_APPLE
        // dlopen() does not add .dylib to the filename, like windows does for .dll
        if (name.substr(name.length() - 6, 6) != ".dylib")
			name += ".dylib";
#elif YUME_PLATFORM == YUME_PLATFORM_WIN32
		// Although LoadLibraryEx will add .dll itself when you only specify the library name,
		// if you include a relative path then it does not. So, add it to be sure.
		if (name.substr(name.length() - 4, 4) != ".dll")
			name += ".dll";
#endif
        mInst = (DYNLIB_HANDLE)DYNLIB_LOAD( (name.c_str()) );

        if( !mInst )
		{
            YUMELOG_FATAL(
                "Could not load dynamic library " + mName +
                ".  System Error: " + dynlibError());
			return false;
		}
		return true;
    }

    //-----------------------------------------------------------------------
    void YumeDynamicLibrary::Unload()
    {
        // Log library unload
        YUMELOG_INFO("Unloading dynamic library " << mName << "....");

        if( DYNLIB_UNLOAD( mInst ) )
		{
			YUMELOG_FATAL(
                "Could not unload dynamic library " + mName +
                ".  System Error: " + dynlibError());
		}
    }

    //-----------------------------------------------------------------------
    void* YumeDynamicLibrary::GetSymbol( const YumeString& strName ) const throw()
    {
        return (void*)DYNLIB_GETSYM( mInst, strName.c_str() );
    }
    //-----------------------------------------------------------------------
    YumeString YumeDynamicLibrary::dynlibError( void )
    {
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
            );
        YumeString ret = (char*)lpMsgBuf;
        // Free the buffer.
        LocalFree( lpMsgBuf );
        return ret;
#elif YUME_PLATFORM == YUME_PLATFORM_LINUX || YUME_PLATFORM == YUME_PLATFORM_APPLE
        return YumeString(dlerror());
#endif
    }
}
