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
#include "Common/Common.h"
#include "Core/YumeEventHub.h"
#include "include/cef_render_handler.h"
#include "include/cef_app.h"
#include "include/cef_client.h"


namespace YumeEngine
{
	class YumeScene;
	class YumeTexture2D;
	class CefTest;
	class BrowserClient;

	class CefTest :
		public BaseApplication,
		public UIEventListener
	{
	public:
		CefTest();
		~CefTest();

		virtual void Start();
		virtual void Setup();

		void MoveCamera(float timeStep);

		virtual void HandleUpdate(float timeStep);
		virtual void HandleBeginFrame(int frameNumber);

		virtual void OnBrowserReady(unsigned int browserIndex);
	};
}

//--------------------------------------------------------------------------------
#endif