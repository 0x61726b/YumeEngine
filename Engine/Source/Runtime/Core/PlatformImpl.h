


























//---------------------------------------------------------------------------------
#pragma once

#ifndef __PlatformImpl_h__
#define __PlatformImpl_h__

#include "YumeRequired.h"
#include <boost/shared_ptr.hpp>


//---------------------------------------------------------------------------------

#	if YUME_PLATFORM == YUME_PLATFORM_WIN32
#		include <Windows.h>
#	    if !defined(WIN32_LEAN_AND_MEAN)
#			define WIN32_LEAN_AND_MEAN
#		endif
#		if !defined(NOMINMAX) && defined(_MSC_VER)
#			define NOMINMAX // required to stop windows.h messing up std::min
#		endif

#		ifdef CreateDirectory
#			undef CreateDirectory
#		endif
#	endif


#endif