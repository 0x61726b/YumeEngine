//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"

#include "BasicRenderWindow.h"

#include "Logging/logging.h"

#include <boost/shared_ptr.hpp>

namespace YumeEngine
{
	HelloWorld::HelloWorld()
	{

	}

	HelloWorld::~HelloWorld()
	{

	}

	void HelloWorld::Start()
	{
		BaseApplication::Start();
	}
}
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int ncmd)
{
#endif
#if YUME_PLATFORM == YUME_PLATFORM_LINUX
int main()
	{
#endif
	

	boost::shared_ptr<YumeEngine::HelloWorld> app(new YumeEngine::HelloWorld);

	return app->Run();
}
