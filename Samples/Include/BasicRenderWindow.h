//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.h
//--------------------------------------------------------------------------------
#ifndef __BasicRenderWindow_h__
#define __BasicRenderWindow_h__
//--------------------------------------------------------------------------------
#include "Core/Yume.h"



namespace YumeSamples
{
	class BasicRenderWindow : public YumeEngine::GeneralObjAlloc,
		public YumeEngine::YumeWindowEventListener
	{
	public:
		BasicRenderWindow();
		~BasicRenderWindow();
		void Setup();


		virtual void OnWindowMoved(YumeEngine::YumeRenderWindow* rw)
		{
			
		}

		virtual bool OnWindowClosing(YumeEngine::YumeRenderWindow* rw)
		{
			return true;
		}

		virtual void OnWindowClosed(YumeEngine::YumeRenderWindow* rw)
		{
			m_Centrum->StopRendering();
#if YUME_PLATFORM == YUME_PLATFORM_WIN32
			PostQuitMessage(0);
#endif
		}

		virtual void OnWindowFocusChange(YumeEngine::YumeRenderWindow* rw)
		{
			
		}


	protected:
		YumeEngine::Yume* m_Centrum;
		YumeEngine::YumeRenderWindow* m_Window;
	};
}

//--------------------------------------------------------------------------------
#endif