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

namespace YumeEngine
{
	class HelloWorld : public BaseApplication
	{
	public:
		HelloWorld();
		~HelloWorld();

		virtual void Start();
		virtual void Setup();
	};
}

//--------------------------------------------------------------------------------
#endif