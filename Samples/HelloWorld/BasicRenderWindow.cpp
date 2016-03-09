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
#include "Core/YumeMain.h"
#include "Scene/YumeOctree.h"
#include <boost/shared_ptr.hpp>

YUME_DEFINE_ENTRY_POINT(YumeEngine::HelloWorld);

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

		scene_ = SharedPtr<YumeScene>(new YumeScene);

		scene_->CreateComponent<Octree>();

	}

	void HelloWorld::Setup()
	{
		BaseApplication::Setup();

		engineVariants_["WindowWidth"] = 1600;
	}
}