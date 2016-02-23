//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"

#include "TestSuite.h"

#include "Logging/logging.h"

#include "Core/YumeMain.h"

#include <boost/shared_ptr.hpp>

YUME_DEFINE_ENTRY_POINT(YumeEngine::TestSuite);

namespace YumeEngine
{



	TestSuite::TestSuite()
	{

	}

	TestSuite::~TestSuite()
	{

	}
	void TestSuite::Stop()
	{

	}

	void TestSuite::Start()
	{
		BaseApplication::Start();
	}
}
