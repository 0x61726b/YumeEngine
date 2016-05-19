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
namespace YumeEngine
{
	class YumeScene;
	class YumeSceneNode;

	class ShadowsDemo : public BaseApplication
	{
	public:
		ShadowsDemo();
		~ShadowsDemo();

		virtual void Start();
		virtual void Setup();

		void MoveCamera(float timeStep);

		virtual void HandleUpdate(float timeStep);

		void CreateLight(Vector3 pos,YumeColor color);

		StaticModel* dragon_;
		float angle_;
	};
}

//--------------------------------------------------------------------------------
#endif
