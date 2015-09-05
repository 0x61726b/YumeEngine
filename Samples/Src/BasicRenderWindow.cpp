//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) Alperen Gezer.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "YumeHeaders.h"

#include "BasicRenderWindow.h"

namespace YumeSamples
{
	BasicRenderWindow::BasicRenderWindow()
	{
		
	}
	BasicRenderWindow::~BasicRenderWindow()
	{
		delete m_Centrum;
	}

	void BasicRenderWindow::Setup()
	{
		m_Centrum = YumeAPINew YumeEngine::YumeCentrum();
		m_Window = m_Centrum->Initialize(true);
		
		if(m_Window)
		{
		    YumeEngine::YumeWindowEvents::AddWindowEventListener(m_Window, this);

		    m_Centrum->StartRendering();
		}
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
	YumeSamples::BasicRenderWindow b;
	b.Setup();

	
	return 0;
}
