//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) Alperen Gezer.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "YumeHeaders.h"
#include "YumeRequired.h"
#include "YumeCentrum.h"

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int ncmd)
{
	YumeEngine::YumeCentrum* yc = YumeAPINew YumeEngine::YumeCentrum;

	
	return 0;
}
#endif
#if YUME_PLATFORM == YUME_PLATFORM_LINUX
#include <sstream>
#include <string>
int main()
{
	
	YumeEngine::YumeString str;
	long numb = YUME_VERSION_NUMBER;
	str.append("Hello from YumeEngine!  Platform Name is Linux! \n");
	str.append("Version: " + YUME_VERSION_MAJOR);
	str.append("." +(numb));
	str.append("\n");
	switch (YUME_SYSTEM_ARCHITECTURE)
	{
	case YUME_SYSTEM_ARCH_32:
		str.append("CPU Architecture=X8664\n");
		break;
	case YUME_SYSTEM_ARCH_64:
		str.append("CPU Architecture=X64\n");
		break;
	default:
		str.append("CPU Architecture=UNKOWN\n");
		break;
	}
	switch (YUME_COMPILER)
	{
	case YUME_COMPILER_MSVC:
		str.append("Compiler=MSVC \n");
		break;
	case YUME_COMPILER_GCCE:
		str.append("Compiler=GCCE \n");
		break;
	case YUME_COMPILER_GNUC:
		str.append("Compiler=GNUC \n");
		break;
	default:
		str.append("Compiler=UNKNOWN \n");
		break;
	}
	str.append("Name: ");
	str.append(YUME_VERSION_NAME);
	std::cout << str << std::endl;

	return 0;
	}
#endif