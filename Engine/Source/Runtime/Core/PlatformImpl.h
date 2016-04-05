










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
// File : <Filename>
// Date : <Date>
// Comments :
//
//---------------------------------------------------------------------------------
#pragma once

#ifndef __PlatformImpl_h__
#define __PlatformImpl_h__

#include "YumeRequired.h"
#include "Core/SharedPtr.h"


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

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
extern YumeAPIExport int GetWndMouseModifiers(WPARAM wparam);
extern YumeAPIExport int GetWndKeyboardModifiers(WPARAM wparam, LPARAM lparam);
extern YumeAPIExport bool IsKeyDown(WPARAM wparam);
extern YumeAPIExport float GetDeviceScaleFactor();
#endif

struct YumeAPIExport GlobalSystems : public YumeEngine::RefCounted
{
	YumeEngine::YumeEngine3D*												pEngine;
	YumeEngine::YumeRHI*													pRHI;
	YumeEngine::SharedPtr<YumeEngine::YumeRenderer>							pRenderer;
	YumeEngine::SharedPtr<YumeEngine::YumeResourceManager>					pResourceManager;
	YumeEngine::SharedPtr<YumeEngine::YumeIO>								pIO;
	YumeEngine::SharedPtr<YumeEngine::YumeTime>								pTimer;
	YumeEngine::SharedPtr<YumeEngine::YumeUI>								pUI;
	YumeEngine::SharedPtr<YumeEngine::YumeWorkQueue>						pWorkSystem;
	YumeEngine::SharedPtr<YumeEngine::YumeEnvironment>						pEnv;
	YumeEngine::SharedPtr<YumeEngine::YumeDebugRenderer>					pDebugRenderer;
	YumeEngine::SharedPtr<YumeEngine::YumeInput>							pInput;
	YumeEngine::SharedPtr<YumeEngine::YumeObjectFactory>					pObjFactory;
	YumeEngine::SharedPtr<YumeEngine::YumePostProcessor>					pPostFx;
};

extern YumeAPIExport YumeEngine::SharedPtr<GlobalSystems> gYume;

#endif